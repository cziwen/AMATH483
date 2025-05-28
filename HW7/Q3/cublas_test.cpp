#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#define CHECK_CUDA(call)                        \
  do {                                          \
    cudaError_t err = (call);                   \
    if (err != cudaSuccess) {                   \
      std::cerr                                 \
        << "CUDA error: "                       \
        << cudaGetErrorString(err)              \
        << " at " << __FILE__ << ":" << __LINE__ \
        << std::endl;                           \
      std::exit(EXIT_FAILURE);                  \
    }                                           \
  } while (0)

#define CHECK_CUBLAS(call)                            \
  do {                                               \
    cublasStatus_t stat = (call);                    \
    if (stat != CUBLAS_STATUS_SUCCESS) {             \
      std::cerr                                      \
        << "cuBLAS error: "                          \
        << stat                                      \
        << " at " << __FILE__ << ":" << __LINE__     \
        << std::endl;                                \
      std::exit(EXIT_FAILURE);                       \
    }                                                \
  } while (0)



int main() {

    int cnt = 0;
    CHECK_CUDA( cudaGetDeviceCount(&cnt) );
    if (cnt == 0) {
        std::cerr << "NO GPU!" << std::endl;
        return 1;
    }

    // 正常运行
    int maxN   = 16384;
    int ntrial =  3;

    // 随机数生成
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    // init cuBLAS
    cublasHandle_t handle;
    CHECK_CUBLAS(cublasCreate(&handle));

    std::cout << "# n gpu_time[s] gpu_gflops\n";

    for (int n = 2; n <= maxN; n *= 2) {
        size_t size = size_t(n) * n;
        std::vector<double> A(size), B(size), C(size);
        for (size_t i = 0; i < size; ++i) {
            A[i] = dist(gen);
            B[i] = dist(gen);
            C[i] = dist(gen);
        }
        const double alpha = 1.0, beta = 1.0;

        // 内存分配 & 拷贝
        double *d_A, *d_B, *d_C;
        CHECK_CUDA(cudaMalloc(&d_A, size * sizeof(double)));
        CHECK_CUDA(cudaMalloc(&d_B, size * sizeof(double)));
        CHECK_CUDA(cudaMalloc(&d_C, size * sizeof(double)));
        CHECK_CUDA(cudaMemcpy(d_A, A.data(), size * sizeof(double), cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(d_B, B.data(), size * sizeof(double), cudaMemcpyHostToDevice));
        CHECK_CUDA(cudaMemcpy(d_C, C.data(), size * sizeof(double), cudaMemcpyHostToDevice));

        // warm-up
        CHECK_CUBLAS(cublasDgemm(handle,
                                 CUBLAS_OP_N, CUBLAS_OP_N,
                                 n, n, n,
                                 &alpha,
                                 d_A, n,
                                 d_B, n,
                                 &beta,
                                 d_C, n));
        CHECK_CUDA(cudaDeviceSynchronize());

        // 正式计时
        float gpu_time = 0.0f;
        cudaEvent_t start, stop;
        CHECK_CUDA(cudaEventCreate(&start));
        CHECK_CUDA(cudaEventCreate(&stop));

        for (int t = 0; t < ntrial; ++t) {
            CHECK_CUDA(cudaMemcpy(d_C, C.data(), size * sizeof(double), cudaMemcpyHostToDevice));
            CHECK_CUDA(cudaEventRecord(start, 0));
            CHECK_CUBLAS(cublasDgemm(handle,
                                     CUBLAS_OP_N, CUBLAS_OP_N,
                                     n, n, n,
                                     &alpha,
                                     d_A, n,
                                     d_B, n,
                                     &beta,
                                     d_C, n));
            CHECK_CUDA(cudaEventRecord(stop, 0));
            CHECK_CUDA(cudaEventSynchronize(stop));
            float ms;
            CHECK_CUDA(cudaEventElapsedTime(&ms, start, stop));
            gpu_time += ms / 1000.0f;
        }
        gpu_time /= ntrial;
        double gpu_gflops = 2.0 * double(n)*n*n / (gpu_time * 1e9);

        // 清理
        CHECK_CUDA(cudaEventDestroy(start));
        CHECK_CUDA(cudaEventDestroy(stop));
        CHECK_CUDA(cudaFree(d_A));
        CHECK_CUDA(cudaFree(d_B));
        CHECK_CUDA(cudaFree(d_C));

        // 输出
        std::cout << n << " " << gpu_time << " " << gpu_gflops << "\n";
    }

    CHECK_CUBLAS(cublasDestroy(handle));
    return 0;
}
#include <iostream>
#include <iomanip>
#include <cuda_runtime.h>
#include <cstdlib>
#include <cstring> // for std::memset

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

int main() {
    const size_t max_bytes = 2ULL * 1024 * 1024 * 1024; // 2GB
    const int ntrial = 3;

    // init cuda events
    cudaEvent_t start, stop;
    CHECK_CUDA(cudaEventCreate(&start));
    CHECK_CUDA(cudaEventCreate(&stop));

    std::cout << "# size(bytes) H2D(bytes/s) D2H(bytes/s)" << std::endl;
    for (size_t size = 1; size <= max_bytes; size *= 2) {
        // allocate pinned host memory and device memory
        void* h_buf = nullptr;
        void* d_buf = nullptr;
        CHECK_CUDA(cudaMallocHost(&h_buf, size));
        CHECK_CUDA(cudaMalloc(&d_buf, size));

        // init host buffer
        std::memset(h_buf, 0, size); // 用 0 填充

        float total_h2d_ms = 0.0f;
        float total_d2h_ms = 0.0f;
        for (int t = 0; t < ntrial; ++t) {
            // H2D
            CHECK_CUDA(cudaEventRecord(start, 0)); // stream 0
            CHECK_CUDA(cudaMemcpy(d_buf, h_buf, size, cudaMemcpyHostToDevice)); // 传输
            CHECK_CUDA(cudaEventRecord(stop, 0)); // stream 0
            CHECK_CUDA(cudaEventSynchronize(stop)); // 等待完成
            float ms_h2d = 0;
            CHECK_CUDA(cudaEventElapsedTime(&ms_h2d, start, stop)); // 计算时间
            total_h2d_ms += ms_h2d;

            // D2H
            CHECK_CUDA(cudaEventRecord(start, 0));
            CHECK_CUDA(cudaMemcpy(h_buf, d_buf, size, cudaMemcpyDeviceToHost)); // 传输
            CHECK_CUDA(cudaEventRecord(stop, 0));
            CHECK_CUDA(cudaEventSynchronize(stop)); // 等待完成
            float ms_d2h = 0;
            CHECK_CUDA(cudaEventElapsedTime(&ms_d2h, start, stop)); // 计算时间
            total_d2h_ms += ms_d2h;
        }
        float avg_h2d_ms = total_h2d_ms / ntrial;
        float avg_d2h_ms = total_d2h_ms / ntrial;
        double h2d_bw = (double)size / (avg_h2d_ms / 1e3);
        double d2h_bw = (double)size / (avg_d2h_ms / 1e3);

        std::cout << std::setw(12) << size << " "
                  << std::fixed << std::setprecision(2)
                  << std::setw(8) << h2d_bw << " "
                  << std::setw(8) << d2h_bw << std::endl;

        // cleanup
        CHECK_CUDA(cudaFree(d_buf));
        CHECK_CUDA(cudaFreeHost(h_buf));
    }
    
    // destory
    CHECK_CUDA(cudaEventDestroy(start));
    CHECK_CUDA(cudaEventDestroy(stop));
    return 0;
}
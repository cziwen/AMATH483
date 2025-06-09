#include <cuda_runtime.h>
#include <cufft.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>

// 线性索引（stride=2）
__host__ __device__ inline size_t id3(int i, int j, int k, int n, int s)
{
    return ((size_t)i * n + j) * n + k;
}

// 频率索引
__host__ __device__ inline int fk(int idx, int n) { return (idx < n / 2 ? idx : idx - n); }

// CPU 端生成平面波
inline void makePlaneWave(std::vector<cufftDoubleComplex> &A, int n, int stride)
{
    size_t M = (size_t)n * n * n * stride;
    for (size_t i = 0; i < M; i++)
    {
        A[i].x = 0;
        A[i].y = 0;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                int p = id3(i, j, k, n, stride);
                double phase = (i + j + k) * 1.0;
                A[p].x = cos(phase);
                A[p].y = sin(phase);
            }
        }
    }
}

// GPU 核：频域乘 i·k
__global__ void mulIk(const cufftDoubleComplex *F, cufftDoubleComplex *Gx,
                      cufftDoubleComplex *Gy, cufftDoubleComplex *Gz,
                      int n, int stride)
{
    int i = blockIdx.x;
    int j = blockIdx.y;
    int k = threadIdx.x;
    if (i < n && j < n && k < n)
    {
        int p = id3(i, j, k, n, stride);
        int kx = fk(i, n), ky = fk(j, n), kz = fk(k, n);
        double a = F[p].x, b = F[p].y;
        Gx[p].x = -kx * b;
        Gx[p].y = kx * a;
        Gy[p].x = -ky * b;
        Gy[p].y = ky * a;
        Gz[p].x = -kz * b;
        Gz[p].y = kz * a;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;
    int n_trial = std::max(3, std::atoi(argv[1]));
    std::vector<int> n_list = {16, 32, 64, 128, 256};
    int stride = 2;
    std::cout << "n,avg_time_sec,flops\n";

    for (int n : n_list)
    {
        size_t N = (size_t)n * n * n;
        size_t M = N * stride;

        // 1) 在 CPU 上生成平面波并分配到 GPU
        std::vector<cufftDoubleComplex> hA(M);
        makePlaneWave(hA, n, stride);

        cufftDoubleComplex *dA, *dF, *dGx, *dGy, *dGz, *dgx, *dgy, *dgz;
        cudaMalloc(&dA, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dF, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dGx, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dGy, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dGz, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dgx, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dgy, M * sizeof(cufftDoubleComplex));
        cudaMalloc(&dgz, M * sizeof(cufftDoubleComplex));
        cudaMemcpy(dA, hA.data(), M * sizeof(cufftDoubleComplex), cudaMemcpyHostToDevice);

        // 2) 创建 cuFFT plan
        cufftHandle p_fwd, p_inv;
        int dims[3] = {n, n, n};
        cufftPlanMany(&p_fwd, 3, dims,
                      dims, stride, N,
                      dims, stride, N,
                      CUFFT_Z2Z, 1);
        cufftPlanMany(&p_inv, 3, dims,
                      dims, stride, N,
                      dims, stride, N,
                      CUFFT_Z2Z, 1);

        float tot = 0;
        for (int t = 0; t < n_trial; t++)
        {
            // 复制初始数据
            cudaMemcpy(dA, hA.data(), M * sizeof(cufftDoubleComplex), cudaMemcpyHostToDevice);

            cudaEvent_t st, ed;
            cudaEventCreate(&st);
            cudaEventCreate(&ed);
            cudaEventRecord(st, 0);

            // 前向 FFT
            cufftExecZ2Z(p_fwd, dA, dF, CUFFT_FORWARD);

            // 频域上乘 i·k
            dim3 grid(n, n);
            dim3 block(n);
            mulIk<<<grid, block>>>(dF, dGx, dGy, dGz, n, stride);

            // 三次逆向 FFT
            cufftExecZ2Z(p_inv, dGx, dgx, CUFFT_INVERSE);
            cufftExecZ2Z(p_inv, dGy, dgy, CUFFT_INVERSE);
            cufftExecZ2Z(p_inv, dGz, dgz, CUFFT_INVERSE);

            cudaEventRecord(ed, 0);
            cudaEventSynchronize(ed);
            float ms;
            cudaEventElapsedTime(&ms, st, ed);
            tot += ms;

            cudaEventDestroy(st);
            cudaEventDestroy(ed);
        }
        double avg = tot / 1000.0 / n_trial;
        double flp = 20.0 * N * log2(N) + 24.0 * N;
        std::cout << n << "," << avg << "," << flp << "\n";

        cufftDestroy(p_fwd);
        cufftDestroy(p_inv);
        cudaFree(dA);
        cudaFree(dF);
        cudaFree(dGx);
        cudaFree(dGy);
        cudaFree(dGz);
        cudaFree(dgx);
        cudaFree(dgy);
        cudaFree(dgz);
    }
    return 0;
}
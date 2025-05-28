#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <openblas/cblas.h>

int main() {
    int maxN   = 16384;
    int ntrial =  3;

    // 随机数生成
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    // 打印表头
    std::cout << "# n cpu_time[s] cpu_gflops\n";

    for (int n = 2; n <= maxN; n *= 2) {
        size_t size = size_t(n) * n;
        // 分配并填充矩阵
        std::vector<double> A(size), B(size), C(size);
        for (size_t i = 0; i < size; ++i) {
            A[i] = dist(gen);
            B[i] = dist(gen);
            C[i] = dist(gen);
        }
        const double alpha = 1.0, beta = 1.0;

        // 计时 CPU 上的 DGEMM
        double cpu_time = 0.0;
        for (int t = 0; t < ntrial; ++t) {
            auto Ccopy = C;
            auto t0 = std::chrono::high_resolution_clock::now();
            cblas_dgemm(
                CblasColMajor, CblasNoTrans, CblasNoTrans,
                n, n, n,
                alpha,
                A.data(), n,
                B.data(), n,
                beta,
                Ccopy.data(), n
            );
            auto t1 = std::chrono::high_resolution_clock::now();
            cpu_time += std::chrono::duration<double>(t1 - t0).count();
        }
        cpu_time /= ntrial;
        double cpu_gflops = 2.0 * double(n) * n * n / (cpu_time * 1e9);

        // 输出结果并换行
        std::cout
          << n << " "
          << cpu_time << " "
          << cpu_gflops
          << "\n";   // ← 不要忘了这个分号和换行
    }

    return 0;
}
#include "ref_dgemv.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

void dgemv(double a, const std ::vector<std ::vector<double>> &A,
           const std ::vector<double> &x, double b, std ::vector<double> &y)
{
    // 计算 a * A * x
    std::vector<double> aAx(A.size(), 0.0);
    for (size_t i = 0; i < A.size(); ++i)
    {
        for (size_t j = 0; j < A[i].size(); ++j)
        {
            aAx[i] += A[i][j] * x[j];
        }
        aAx[i] *= a;
    }

    // 计算整体
    for (size_t i = 0; i < y.size(); ++i)
    {
        y[i] = aAx[i] + b * y[i];
    }
}

// 测试并输出 Level-2 BLAS dgemv 性能
void test_dgemv(int n, int ntrials)
{
    std::vector<std::vector<double>> A(n, std::vector<double>(n));
    std::vector<double> x(n), y(n);
    double alpha = 2.5, beta = 0.5;
    // const int ntrials = 5;
    long double elapsed_time = 0.L;
    long double avg_time;
    auto start = std::chrono::high_resolution_clock::now();
    auto stop  = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration;

    // 随机初始化 A, x, y
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(1.0, 2.0);
    for (int i = 0; i < n; ++i)
    {
        x[i] = dist(rng);
        y[i] = dist(rng);
        for (int j = 0; j < n; ++j)
        {
            A[i][j] = dist(rng);
        }
    }

    for (int t = 0; t < ntrials; ++t) {
        std::vector<double> y_copy = y;
        start = std::chrono::high_resolution_clock::now();
        dgemv(alpha, A, x, beta, y_copy);
        stop  = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        elapsed_time += (duration.count() * 1.e-9);
    }
    avg_time = elapsed_time / static_cast<long double>(ntrials);
    double flops = 2.0 * n * n / avg_time;
    std::cout << "dgemv n = " << n
              << ", Trials = " << ntrials
              << ", Avg Time = " << avg_time << "s"
              << ", Flops = " << flops
              << ", GFLOPs = " << flops / 1e9 << std::endl;
}

// int main()
// {
//     for (int n = 2; n <= 512; n *= 2) // n = 2, 4, 8, ..., 512
//     {
//         test_dgemv(n);
//     }

//     return 0;
// }

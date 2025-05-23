#include "ref_dgemm.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

// void dgemm(double a, const std ::vector<std ::vector<double>> &A,
//            const std ::vector<std ::vector<double>> &B, double b,
//            std ::vector<std ::vector<double>> &C)
// {
//     // 计算 A*B
//     std::vector<std::vector<double>> aAB(A.size(), std::vector<double>(B[0].size(), 0.0));
//     for (size_t i = 0; i < A.size(); ++i)
//     {
//         for (size_t j = 0; j < B[0].size(); ++j)
//         {
//             for (size_t k = 0; k < A[0].size(); ++k)
//             {
//                 aAB[i][j] += A[i][k] * B[k][j];
//             }
//             aAB[i][j] *= a;
//         }
//     }

//     // 计算整体
//     for (size_t i = 0; i < C.size(); ++i)
//     {
//         for (size_t j = 0; j < C[0].size(); ++j)
//         {
//             C[i][j] = aAB[i][j] + b * C[i][j];
//         }
//     }
// }

void dgemm(double a, const std::vector<std::vector<double>> &A,
           const std::vector<std::vector<double>> &B, double b,
           std::vector<std::vector<double>> &C)
{
    size_t m = A.size();    // A 的行数
    size_t n = B[0].size(); // B 的列数
    size_t p = A[0].size(); // A 的列数 == B 的行数

    for (size_t i = 0; i < m; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            double sum = 0.0;
            for (size_t k = 0; k < p; ++k)
            {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = a * sum + b * C[i][j];
        }
    }
}

// 测试并输出每秒浮点运算次数 FLOPs
void test_dgemm(int n, int ntrial)
{
    std::vector<std::vector<double>> A(n, std::vector<double>(n));
    std::vector<std::vector<double>> B(n, std::vector<double>(n));
    std::vector<std::vector<double>> C(n, std::vector<double>(n));
    double alpha = 1.0, beta = 1.0;

    // 随机初始化 A, B, C
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            A[i][j] = dist(rng);
            B[i][j] = dist(rng);
            C[i][j] = dist(rng);
        }
    }

    long double elapsed_time = 0.0L;
    std::chrono::high_resolution_clock::time_point start, stop;
    std::chrono::nanoseconds duration;

    for (int t = 0; t < ntrial; ++t)
    {
        std::vector<std::vector<double>> C_copy = C;

        start = std::chrono::high_resolution_clock::now();
        dgemm(alpha, A, B, beta, C_copy);
        stop = std::chrono::high_resolution_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        elapsed_time += duration.count() * 1.e-9; // convert to seconds
    }

    long double avg_time = elapsed_time / static_cast<long double>(ntrial);
    double flops = 2.0 * n * n * n / avg_time;

    std::cout << "dgemm n = " << n
              << ", Avg Time = " << avg_time << "s"
              << ", FLOPs = " << flops
              << ", GFLOPs = " << flops / 1e9
              << std::endl;
}

// int main() {
//     int ntrial = 5;
//     for (int n = 2; n <= 512; n *= 2) {
//         test_dgemm(n, ntrial);
//     }
//     return 0;
// }
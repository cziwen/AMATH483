#include "ref_axpyt.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

template <typename T>
void axpy(T a, const std::vector<T> &x, std::vector<T> &y)
{
    size_t n = x.size();
    for (size_t i = 0; i < n; ++i) {
        y[i] += a * x[i];
    }
}

// 测试并输出每秒浮点运算次数 FLOPs
void test_axpy(int n, int ntrials)
{
    std::vector<double> x(n), y(n);
    double a = 2.5;
    // const int ntrials = 5;
    long double elapsed_time = 0.L;
    long double avg_time;
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration;

    for (int t = 0; t < ntrials; ++t)
    {
        std::vector<double> y_copy = y;
        start = std::chrono::high_resolution_clock::now();
        axpy(a, x, y_copy);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        elapsed_time += (duration.count() * 1.e-9); // Convert duration to seconds
    }
    avg_time = elapsed_time / static_cast<long double>(ntrials);
    double flops = 2.0 * n / avg_time;
    std::cout << "daxpy n = " << n
              << ", Trials = " << ntrials
              << ", Avg Time = " << avg_time << "s"
              << ", Flops = " << flops
              << ", GFLOPs = " << flops / 1e9 << std::endl;
}

// // 主函数：从 n=2 到 n=512 测试
// int main()
// {
//     for (int n = 2; n <= 512; n *= 2)
//     { // n = 2, 4, 8, ..., 512
//         test_axpy(n);
//     }

//     return 0;
// }
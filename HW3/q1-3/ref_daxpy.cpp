#include "ref_daxpy.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

// daxpy 实现：y = a * x + y
void daxpy(double a, const std::vector<double>& x, std::vector<double>& y) {
    size_t n = x.size();
    for (size_t i = 0; i < n; ++i) {
        y[i] += a * x[i];
    }
}

// 测试并输出每秒浮点运算次数 FLOPs
void test_daxpy(int n, int ntrials) {
    std::vector<double> x(n), y(n);
    double a = 2.5;
    long double elapsed_time = 0.L;
    long double avg_time;
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration;

    for (int t = 0; t < ntrials; ++t) {
        std::vector<double> y_copy = y;
        start = std::chrono::high_resolution_clock::now();
        daxpy(a, x, y_copy);
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

// 主函数：从 n=2 到 n=512 测试
// int main() {
//     int ntrials = 5; // 试验次数
//     for (int n = 2; n <= 512; n *= 2) { // n = 2, 4, 8, ..., 512
//         test_daxpy(n, ntrials); // 5 次试验
//     }

//     return 0;
// }
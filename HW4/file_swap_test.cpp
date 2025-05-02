#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include "file_swaps.hpp"

int main() {
    std::vector<int> sizes = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int ntrial = 5;

    std::mt19937 rng(42);  // 固定随机种子
    std::uniform_int_distribution<> dist; // 随机分布

    for (int N : sizes) {
        double total_row_time = 0.0;
        double total_col_time = 0.0;
        std::string filename = "matrix_" + std::to_string(N) + ".bin";

        for (int t = 0; t < ntrial; ++t) {
            // 生成矩阵（column-major order）
            std::vector<double> matrix(N * N);
            for (int col = 0; col < N; ++col) {
                for (int row = 0; row < N; ++row) {
                    matrix[col * N + row] = static_cast<double>(col * N + row);
                }
            }

            // 写入文件
            std::ofstream fout(filename, std::ios::binary);
            fout.write(reinterpret_cast<const char *>(matrix.data()), N * N * sizeof(double));
            fout.close();

            // 随机选取两个行号/列号
            dist = std::uniform_int_distribution<>(0, N - 1);
            int i = dist(rng);
            int j = dist(rng);
            while (j == i) j = dist(rng); // 确保不相等

            // ----------- 行交换 ------------
            std::fstream file_row(filename, std::ios::in | std::ios::out | std::ios::binary);
            auto start_row = std::chrono::high_resolution_clock::now();
            swapRowsInFile(file_row, N, N, i, j);
            auto end_row = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_row = end_row - start_row;
            total_row_time += elapsed_row.count();
            file_row.close();

            // ----------- 列交换 ------------
            std::fstream file_col(filename, std::ios::in | std::ios::out | std::ios::binary);
            auto start_col = std::chrono::high_resolution_clock::now();
            swapColsInFile(file_col, N, N, i, j);
            auto end_col = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_col = end_col - start_col;
            total_col_time += elapsed_col.count();
            file_col.close();
        }

        // 删除测试文件
        std::remove(filename.c_str());

        // 打印结果：N vs time
        std::cout << "N=" << N
                  << ", row time: " << (total_row_time / ntrial)
                  << ", col time: " << (total_col_time / ntrial)
                  << std::endl;
    }

    return 0;
}
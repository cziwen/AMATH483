#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>

void write_matrix(const std::vector<double>& matrix, int N, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < N; ++row) {
            double val = matrix[row + col * N]; // column-major
            out.write(reinterpret_cast<const char*>(&val), sizeof(double));
        }
    }
    
}

void read_matrix(std::vector<double>& matrix, int N, const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < N; ++row) {
            double val;
            in.read(reinterpret_cast<char*>(&val), sizeof(double));
            matrix[row + col * N] = val;
        }
    }
}


int main() {
    std::vector<int> sizes = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};

    for (int N : sizes) {
        std::vector<double> matrix(N * N, 1.23);  // 初始化
        std::string filename = "q5_matrix_" + std::to_string(N) + ".bin";

        // write
        auto start_write = std::chrono::high_resolution_clock::now();
        write_matrix(matrix, N, filename);
        auto end_write = std::chrono::high_resolution_clock::now();
        double write_time = std::chrono::duration<double>(end_write - start_write).count();

        // read
        std::vector<double> loaded(N * N);
        auto start_read = std::chrono::high_resolution_clock::now();
        read_matrix(loaded, N, filename);
        auto end_read = std::chrono::high_resolution_clock::now();
        double read_time = std::chrono::duration<double>(end_read - start_read).count();

        double total_bytes = static_cast<double>(N) * N * sizeof(double);
        double write_bandwidth = total_bytes / write_time;
        double read_bandwidth = total_bytes / read_time;

        std::cout << N << " " << write_bandwidth << " " << read_bandwidth << std::endl;

        // Clean up
        std::remove(filename.c_str());
    }
}
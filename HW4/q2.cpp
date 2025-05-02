#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>

template<typename T>
void mm_kij(T a, const std::vector<T>& A, const std::vector<T>& B, T b, std::vector<T>& C, int m, int p, int n) {
    for (int k = 0; k < p; ++k)
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                C[i*n + j] = b * C[i*n + j] + a * A[i*p + k] * B[k*n + j];
}

template<typename T>
void mm_jki(T a, const std::vector<T>& A, const std::vector<T>& B, T b, std::vector<T>& C, int m, int p, int n) {
    for (int j = 0; j < n; ++j)
        for (int k = 0; k < p; ++k)
            for (int i = 0; i < m; ++i)
                C[i*n + j] = b * C[i*n + j] + a * A[i*p + k] * B[k*n + j];
}

template<typename T>
void random_matrix(std::vector<T>& M, int size) {
    std::mt19937 gen(42);  // fixed seed for reproducibility
    std::uniform_real_distribution<T> dist(0.0, 1.0);
    for (auto& val : M)
        val = dist(gen);
}

template<typename T>
double benchmark(void (*mm_func)(T, const std::vector<T>&, const std::vector<T>&, T, std::vector<T>&, int, int, int),
                 int n, int ntrial) {
    std::vector<T> A(n * n), B(n * n), C(n * n);
    random_matrix(A, n * n);
    random_matrix(B, n * n);
    random_matrix(C, n * n);

    auto start = std::chrono::high_resolution_clock::now();
    for (int t = 0; t < ntrial; ++t) {
        std::vector<T> C_copy = C;  // reset C each trial
        mm_func((T)1.0, A, B, (T)0.0, C_copy, n, n, n);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    double avg_time = elapsed.count() / ntrial; // average time per trial
    double flops = 2.0 * n * n * n / avg_time; // 2*n^3 FLOPs
    return flops;
}

int main() {
    int ntrial = 5;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "n\tkij_FLOPs(float)\tjki_FLOPs(float)\n";

    // test for float
    for (int n = 2; n <= 512; n *= 2) {
        double flops_kij = benchmark<float>(mm_kij<float>, n, ntrial);
        double flops_jki = benchmark<float>(mm_jki<float>, n, ntrial);
        std::cout << n << "\t" << flops_kij << " FLOPs\t\t" << flops_jki << " FLOPs\n";
    }



    std::cout << std::fixed << std::setprecision(2);
    std::cout << "n\tkij_FLOPs(double)\tjki_FLOPs(double)\n";
    // test for doubles
    for (int n = 2; n <= 512; n *= 2) {
        double flops_kij = benchmark<double>(mm_kij<double>, n, ntrial);
        double flops_jki = benchmark<double>(mm_jki<double>, n, ntrial);
        std::cout << n << "\t" << flops_kij << " FLOPs\t\t" << flops_jki << " FLOPs\n";
    }
    return 0;
}
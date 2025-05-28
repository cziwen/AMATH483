#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "my_cblas.h"

// 向上对齐字节数到 64 的倍数
size_t aligned_bytes(size_t n_elem) {
    size_t bytes = n_elem * sizeof(double);
    return ((bytes + 63) / 64) * 64;
}

void benchmark_daxpy(int n) {
    size_t bytes = aligned_bytes(n);
    double* X = static_cast<double*>(aligned_alloc(64, bytes));
    double* Y = static_cast<double*>(aligned_alloc(64, bytes));
    if (!X || !Y) {
        std::cerr << "daxpy: Memory allocation failed!" << std::endl;
        std::exit(1);
    }

    for (int i = 0; i < n; ++i) {
        X[i] = rand() / (double)RAND_MAX;
        Y[i] = rand() / (double)RAND_MAX;
    }

    auto start = std::chrono::high_resolution_clock::now();
    cblas_daxpy(n, 1.0, X, 1, Y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration<double>(end - start).count();
    double flops    = 2.0 * n;
    std::cout << "daxpy: n=" << n
              << " time=" << duration << " s"
              << "  MFLOPS=" << (flops / duration / 1e6)
              << std::endl;

    free(X);
    free(Y);
}

void benchmark_dgemv(int n) {
    size_t mat_bytes = aligned_bytes(n * n);
    size_t vec_bytes = aligned_bytes(n);

    double* A = static_cast<double*>(aligned_alloc(64, mat_bytes));
    double* X = static_cast<double*>(aligned_alloc(64, vec_bytes));
    double* Y = static_cast<double*>(aligned_alloc(64, vec_bytes));
    if (!A || !X || !Y) {
        std::cerr << "dgemv: Memory allocation failed!" << std::endl;
        std::exit(1);
    }

    for (int i = 0; i < n*n; ++i) A[i] = rand() / (double)RAND_MAX;
    for (int i = 0; i < n;   ++i) X[i] = Y[i] = rand() / (double)RAND_MAX;

    auto start = std::chrono::high_resolution_clock::now();
    cblas_dgemv(CblasColMajor, CblasNoTrans,
                n, n,
                1.0, A, n,
                     X, 1,
                0.0, Y, 1);
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration<double>(end - start).count();
    double flops    = 2.0 * n * n;
    std::cout << "dgemv: n=" << n
              << " time=" << duration << " s"
              << "  MFLOPS=" << (flops / duration / 1e6)
              << std::endl;

    free(A);
    free(X);
    free(Y);
}

void benchmark_dgemm(int n) {
    size_t mat_bytes = aligned_bytes(n * n);

    double* A = static_cast<double*>(aligned_alloc(64, mat_bytes));
    double* B = static_cast<double*>(aligned_alloc(64, mat_bytes));
    double* C = static_cast<double*>(aligned_alloc(64, mat_bytes));
    if (!A || !B || !C) {
        std::cerr << "dgemm: Memory allocation failed!" << std::endl;
        std::exit(1);
    }

    for (int i = 0; i < n*n; ++i) {
        A[i] = rand() / (double)RAND_MAX;
        B[i] = rand() / (double)RAND_MAX;
        C[i] = 0.0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                n, n, n,
                1.0, A, n,
                     B, n,
                0.0, C, n);
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration<double>(end - start).count();
    double flops    = 2.0 * n * n * n;
    std::cout << "dgemm: n=" << n
              << " time=" << duration << " s"
              << "  MFLOPS=" << (flops / duration / 1e6)
              << std::endl;

    free(A);
    free(B);
    free(C);
}

int main() {
    std::srand(std::time(nullptr));
    
    for (int n = 2; n <= 4096; n *= 2) {
        benchmark_daxpy(n);
        benchmark_dgemv(n);
        benchmark_dgemm(n);
        std::cout << "--------------------------------------" << std::endl;
    }

    return 0;
}
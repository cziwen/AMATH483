#pragma once
#include <vector>


void daxpy(double a, const std::vector<double> &x, std::vector<double> &y);

void dgemv(double a, const std::vector<std::vector<double>> &A,
           const std::vector<double> &x, double b, std::vector<double> &y);

void dgemm(double a, const std::vector<std::vector<double>> &A,
           const std::vector<std::vector<double>> &B, double b,
           std::vector<std::vector<double>> &C);

template <typename T>
void axpy(T a, const std::vector<T> &x, std::vector<T> &y);

template <typename T>
void gemv(T a, const std::vector<std::vector<T>> &A,
          const std::vector<T> &x, T b, std::vector<T> &y);

template <typename T>
void gemm(T a, const std::vector<std::vector<T>> &A,
          const std::vector<std::vector<T>> &B, T b,
          std::vector<std::vector<T>> &C);
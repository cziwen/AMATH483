#include "librefBlas.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>



int main() {
    // Data initialization ==========================
    // You may modify the data here for testing, Each function will be tested with the same data.
    const std::vector<double> baseX = std::vector<double>{1.0, 2.0, 3.0};
    const std::vector<double> baseY = std::vector<double>{4.0, 5.0, 6.0};
    const std::vector<std::vector<double> > baseA = std::vector<std::vector<double> >{
        std::vector<double>{1.0, 0.0, 2.0},
        std::vector<double>{0.0, 1.0, 0.0},
        std::vector<double>{3.0, 0.0, 1.0}
    };
    const std::vector<std::vector<double> > baseB = std::vector<std::vector<double> >{
        std::vector<double>{1.0, 2.0, 1.0},
        std::vector<double>{0.0, 1.0, 0.0},
        std::vector<double>{4.0, 0.0, 3.0}
    };

    std::vector<double> x = baseX;
    std::vector<double> y = baseY;
    std::vector<std::vector<double> > A = baseA;
    std::vector<std::vector<double> > B = baseB;
    std::vector<std::vector<double> > C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));






    // Actual function calls ==========================
    // 调用模板函数 axpy
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    axpy(2.0, x, y);
    std::cout << "Result of axpy: ";
    for (auto val : y) std::cout << val << " ";
    std::cout << std::endl;

    // 调用模板函数 gemv
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    y = std::vector<double>{0.0, 0.0, 0.0};
    gemv(1.0, A, x, 0.0, y);
    std::cout << "Result of gemv: ";
    for (auto val : y) std::cout << val << " ";
    std::cout << std::endl;

    // 调用模板函数 gemm
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    gemm(1.0, A, B, 0.0, C);
    std::cout << "Result of gemm:" << std::endl;
    for (auto& row : C) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }

    // 调用非模板函数 daxpy
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    daxpy(2.0, x, y);
    std::cout << "Result of daxpy: ";
    for (auto val : y) std::cout << val << " ";
    std::cout << std::endl;

    // 调用非模板函数 dgemv
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    y = std::vector<double>{0.0, 0.0, 0.0};
    dgemv(1.0, A, x, 0.0, y);
    std::cout << "Result of dgemv: ";
    for (auto val : y) std::cout << val << " ";
    std::cout << std::endl;

    // 调用非模板函数 dgemm
    x = baseX;
    y = baseY;
    A = baseA;
    B = baseB;
    C = std::vector<std::vector<double> >(3, std::vector<double>(3, 0.0));
    dgemm(1.0, A, B, 0.0, C);
    std::cout << "Result of dgemm:" << std::endl;
    for (auto& row : C) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }

    return 0;
}

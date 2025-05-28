#include <iostream>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <limits>
#include <cblas.h>
#include <lapacke.h>

int main() {
    // 1) 循环遍历维度：16,32,64,…,8192
    for(int n = 16; n <= 8192; n *= 2) {
        int ma = n, na = n, nrhs = 1;
        // 2) 分配 A, A_work, b, b_orig, ipiv
        auto A    = (std::complex<double>*)malloc(sizeof(std::complex<double>) * n * n);
        auto Awrk = (std::complex<double>*)malloc(sizeof(std::complex<double>) * n * n);
        auto b    = (std::complex<double>*)malloc(sizeof(std::complex<double>) * n);
        auto b0   = (std::complex<double>*)malloc(sizeof(std::complex<double>) * n);
        auto ipiv = (lapack_int*)malloc(sizeof(lapack_int) * n);
        // 3) 填充 A, b
        srand(0);
        for(int j=0,k=0; j<na; ++j)
            for(int i=0; i<ma; ++i,++k) {
                double x = 0.5 - double(rand())/RAND_MAX;
                double y = 0.5 - double(rand())/RAND_MAX;
                std::complex<double> z1(x,y);
                if(i==j) z1 *= double(ma);
                A[k] = z1;
            }
        srand(1);
        for(int i=0; i<ma; ++i) {
            double x = 0.5 - double(rand())/RAND_MAX;
            double y = 0.5 - double(rand())/RAND_MAX;
            b[i] = std::complex<double>(x,y);
        }
        // 4) 备份原始 A 和 b
        std::memcpy(Awrk, A, sizeof(std::complex<double>)*n*n);
        std::memcpy(b0, b, sizeof(std::complex<double>)*n);


        // 5) 调用 LAPACKE_zgesv 解线性方程 Awrk * x = b
        lapack_int info = LAPACKE_zgesv(LAPACK_COL_MAJOR, n, nrhs,
                                       (lapack_complex_double*)Awrk, n,
                                       ipiv,
                                       (lapack_complex_double*)b, n);
        if(info != 0) {
            std::cerr << "zgesv failed at n="<<n<<" info="<<info<<"\n";
            break;
        }

        // 6) 计算残差 r = b0 - A * x
        std::vector<std::complex<double>> r(n);
        for(int i=0; i<n; ++i) {
            std::complex<double> s = 0.0;
            for(int j=0; j<n; ++j)
                s += A[i + j*n] * b[j];
            r[i] = b0[i] - s;
        }

        // 7) 计算范数
        // 7.1 2-范数：||r||_2, ||x||_2
        double res_norm = cblas_dznrm2(n, (const lapack_complex_double*)r.data(), 1);
        double x_norm   = cblas_dznrm2(n, (const lapack_complex_double*)b,      1);
        // 7.2 ∞-范数：||A||_∞ = max_i ∑_j |A_ij|
        double A_inf = 0.0;
        for(int i=0; i<n; ++i) {
            double row_sum = 0.0;
            for(int j=0; j<n; ++j)
                row_sum += std::abs(A[i + j*n]);
            A_inf = std::max(A_inf, row_sum);
        }
        // 7.3 机器精度 ε_machine
        double eps = std::numeric_limits<double>::epsilon();

        // 8) 归一化误差
        double norm_err = res_norm / (A_inf * x_norm * eps);

        // 9) 输出：n, log(res_norm), log(norm_err)
        std::cout << n << " "
                  << std::log10(res_norm) << " "
                  << std::log10(norm_err) << "\n";

        // 10) 释放
        free(A); free(Awrk); free(b); free(b0); free(ipiv);
    }
    return 0;
}
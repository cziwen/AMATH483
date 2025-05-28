#ifndef MINI_CBLAS_H
#define MINI_CBLAS_H

#ifdef __cplusplus
extern "C" {
#endif

// Minimal CBLAS enums needed
enum CBLAS_ORDER { CblasRowMajor = 101, CblasColMajor = 102 };
enum CBLAS_TRANSPOSE { CblasNoTrans = 111, CblasTrans = 112, CblasConjTrans = 113 };

// Level 1
void cblas_daxpy(const int N, const double alpha,
                 const double *X, const int incX,
                 double *Y, const int incY);

// Level 2
void cblas_dgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA,
                 const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 const double *X, const int incX,
                 const double beta, double *Y, const int incY);

// Level 3
void cblas_dgemm(const enum CBLAS_ORDER Order,
                 const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB,
                 const int M, const int N, const int K,
                 const double alpha, const double *A, const int lda,
                 const double *B, const int ldb,
                 const double beta, double *C, const int ldc);

#ifdef __cplusplus
}
#endif

#endif
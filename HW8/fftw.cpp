#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>

inline size_t id3(int i, int j, int k, int n, int s)
{
    return ((size_t)i * n + j) * n + k;
}
inline int fk(int idx, int n) { return (idx < n / 2 ? idx : idx - n); }

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 1;
    int n_trial = std::max(3, std::atoi(argv[1]));
    std::vector<int> n_list = {16, 32, 64, 128, 256};
    int stride = 2;
    std::cout << "n,avg_time_sec,flops\n";
    for (int n : n_list)
    {
        size_t N = (size_t)n * n * n;
        size_t M = N * stride;
        // 分配
        auto malloc_c = [&](void **p)
        { *p = fftw_malloc(sizeof(fftw_complex)*M); if(!*p) exit(1); };
        fftw_complex *in, *raw, *F, *Gx, *Gy, *Gz, *gx, *gy, *gz;
        malloc_c((void **)&in);
        malloc_c((void **)&raw);
        malloc_c((void **)&F);
        malloc_c((void **)&Gx);
        malloc_c((void **)&Gy);
        malloc_c((void **)&Gz);
        malloc_c((void **)&gx);
        malloc_c((void **)&gy);
        malloc_c((void **)&gz);
        // 初始化 raw (平面波)
        std::memset(raw, 0, sizeof(fftw_complex) * M);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int k = 0; k < n; k++)
                {
                    size_t p = id3(i, j, k, n, stride);
                    double phase = (i + j + k) * 1.0;
                    raw[p][0] = std::cos(phase);
                    raw[p][1] = std::sin(phase);
                }
            }
        }
        // 创建 plan
        int dims[3] = {n, n, n};
        fftw_plan pf = fftw_plan_many_dft(3, dims, 1,
                                          in, dims, stride, N,
                                          F, dims, stride, N,
                                          FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_plan pbx = fftw_plan_many_dft(3, dims, 1,
                                           Gx, dims, stride, N,
                                           gx, dims, stride, N,
                                           FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_plan pby = fftw_plan_many_dft(3, dims, 1,
                                           Gy, dims, stride, N,
                                           gy, dims, stride, N,
                                           FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_plan pbz = fftw_plan_many_dft(3, dims, 1,
                                           Gz, dims, stride, N,
                                           gz, dims, stride, N,
                                           FFTW_BACKWARD, FFTW_ESTIMATE);
        // 测时
        double total = 0;
        for (int t = 0; t < n_trial; t++)
        {
            std::memcpy(in, raw, sizeof(fftw_complex) * M);
            auto t0 = std::chrono::high_resolution_clock::now();
            fftw_execute(pf);
            // 构造 Gx,Gy,Gz
            for (int i = 0; i < n; i++)
            {
                int kx = fk(i, n);
                for (int j = 0; j < n; j++)
                {
                    int ky = fk(j, n);
                    for (int k = 0; k < n; k++)
                    {
                        int kz = fk(k, n);
                        size_t p = id3(i, j, k, n, stride);
                        double a = F[p][0], b = F[p][1];
                        Gx[p][0] = -kx * b;
                        Gx[p][1] = kx * a;
                        Gy[p][0] = -ky * b;
                        Gy[p][1] = ky * a;
                        Gz[p][0] = -kz * b;
                        Gz[p][1] = kz * a;
                    }
                }
            }
            fftw_execute(pbx);
            fftw_execute(pby);
            fftw_execute(pbz);
            auto t1 = std::chrono::high_resolution_clock::now();
            total += std::chrono::duration<double>(t1 - t0).count();
        }
        double avg = total / n_trial;
        double flp = 20.0 * N * std::log2(N) + 24.0 * N;
        std::cout << n << "," << avg << "," << flp << "\n";
        fftw_destroy_plan(pf);
        fftw_destroy_plan(pbx);
        fftw_destroy_plan(pby);
        fftw_destroy_plan(pbz);
        fftw_free(in);
        fftw_free(raw);
        fftw_free(F);
        fftw_free(Gx);
        fftw_free(Gy);
        fftw_free(Gz);
        fftw_free(gx);
        fftw_free(gy);
        fftw_free(gz);
    }
    return 0;
}
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "strassen-1.cpp"

using namespace std;
using namespace std::chrono;

vector<vector<double>> generateRandomMatrix(int n)
{
    mt19937 gen(42);
    uniform_real_distribution<> dis(0.0, 1.0);
    vector<vector<double>> mat(n, vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = dis(gen);
    return mat;
}

int main()
{
    vector<int> sizes = {
        2, 3, 4, 6, 8, 12, 16, 24, 32,
        48, 64, 96, 128, 192, 256, 384, 512
    };
    int ntrial = 3;

    cout << "# size\tFLOPs" << endl;

    for (int n : sizes)
    {
        double total_time = 0.0;

        for (int t = 0; t < ntrial; ++t)
        {
            auto A = generateRandomMatrix(n);
            auto B = generateRandomMatrix(n);

            auto start = high_resolution_clock::now();
            auto C = strassenMultiply(A, B);
            auto end = high_resolution_clock::now();

            auto duration = duration_cast<std::chrono::duration<double>>(end - start);
            total_time += duration.count();
        }

        double avg_time = total_time / ntrial;
        double estimated_flops = 2.0 * pow(n, 2.81); // Strassen 的近似复杂度, not exact
        double flops_performance = estimated_flops / avg_time; // 单位：FLOPs/sec

        cout << n << "\t" << fixed << setprecision(5) << flops_performance << endl;
    }

    return 0;
}


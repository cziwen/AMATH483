#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <utility>
#include "matrix_swaps.hpp"

std::pair<int, int> getRandomIndices(int n) {
    int i = std::rand() % n;
    int j = std::rand() % (n - 1);
    if (j >= i) ++j;
    return std::make_pair(i, j);
}

void runTest(int size, int ntrial) {
    int nRows = size, nCols = size;
    std::vector<double> matrix(nRows * nCols);
    for (int i = 0; i < nRows * nCols; ++i) {
        matrix[i] = static_cast<double>(i);
    }

    double rowSwapTotalTime = 0.0;
    double colSwapTotalTime = 0.0;

    for (int t = 0; t < ntrial; ++t) {
        auto rowIdx = getRandomIndices(nRows);
        auto start = std::chrono::high_resolution_clock::now();
        swapRows(matrix, nRows, nCols, rowIdx.first, rowIdx.second);
        auto end = std::chrono::high_resolution_clock::now();
        rowSwapTotalTime += std::chrono::duration<double>(end - start).count();

        auto colIdx = getRandomIndices(nCols);
        start = std::chrono::high_resolution_clock::now();
        swapCols(matrix, nRows, nCols, colIdx.first, colIdx.second);
        end = std::chrono::high_resolution_clock::now();
        colSwapTotalTime += std::chrono::duration<double>(end - start).count();
    }

    double avgRowTime = rowSwapTotalTime / ntrial;
    double avgColTime = colSwapTotalTime / ntrial;

    std::cout << size << " " << avgRowTime << " " << avgColTime << std::endl;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int ntrial = 5;
    std::vector<int> sizes = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

    std::cout << "# size row_time col_time\n";
    for (int size : sizes) {
        runTest(size, ntrial);
    }

    return 0;
}
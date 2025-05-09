#include <vector>
#include <algorithm>

void swapRows(std::vector<double> &matrix, int nRows, int nCols, int i, int j) {
    if (i == j) return;

    for (int col = 0; col < nCols; ++col) {
        double &a = matrix[col * nRows + i];
        double &b = matrix[col * nRows + j];

        std::swap(a, b);
    }
}

void swapCols(std::vector<double> &matrix, int nRows, int nCols, int i, int j) {
    if (i == j) return;

    for (int row = 0; row < nRows; ++row) {
        double &a = matrix[i * nRows + row];
        double &b = matrix[j * nRows + row];
        std::swap(a, b);
    }
}
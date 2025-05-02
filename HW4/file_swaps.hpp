#ifndef FILE_SWAPS_HPP
#define FILE_SWAPS_HPP

#include <fstream>
#include <vector>

void swapRowsInFile(std::fstream &file, int nRows, int nCols, int i, int j) {
    std::vector<double> buffer(nCols);
    for (int col = 0; col < nCols; ++col) {
        file.seekg((col * nRows + i) * sizeof(double), std::ios::beg); // at the ith row
        file.read(reinterpret_cast<char*>(&buffer[col]), sizeof(double));

        double temp;
        file.seekg((col * nRows + j) * sizeof(double), std::ios::beg); // at the jth row
        file.read(reinterpret_cast<char*>(&temp), sizeof(double));

        // Swap in file
        file.seekp((col * nRows + i) * sizeof(double), std::ios::beg);
        file.write(reinterpret_cast<char*>(&temp), sizeof(double));
        file.seekp((col * nRows + j) * sizeof(double), std::ios::beg);
        file.write(reinterpret_cast<char*>(&buffer[col]), sizeof(double));
    }
}

void swapColsInFile(std::fstream &file, int nRows, int nCols, int i, int j) {
    std::vector<double> buffer(nRows);
    for (int row = 0; row < nRows; ++row) {
        file.seekg((i * nRows + row) * sizeof(double), std::ios::beg); // at the ith column
        file.read(reinterpret_cast<char*>(&buffer[row]), sizeof(double));

        double temp;
        file.seekg((j * nRows + row) * sizeof(double), std::ios::beg); // at the jth column
        file.read(reinterpret_cast<char*>(&temp), sizeof(double));

        // Swap in file
        file.seekp((i * nRows + row) * sizeof(double), std::ios::beg);
        file.write(reinterpret_cast<char*>(&temp), sizeof(double));
        file.seekp((j * nRows + row) * sizeof(double), std::ios::beg);
        file.write(reinterpret_cast<char*>(&buffer[row]), sizeof(double));
    }
}

#endif
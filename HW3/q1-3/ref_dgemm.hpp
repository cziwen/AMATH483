#pragma once
#include <vector>

void dgemm(double a, const std ::vector<std ::vector<double>> &A,
           const std ::vector<std ::vector<double>> &B, double b,
           std ::vector<std ::vector<double>> &C);
#pragma once
#include <vector>

template <typename T>
void dgemv(T a, const std ::vector<std ::vector<T>> &A,
           const std ::vector<T> &x, T b, std ::vector<T> &y);
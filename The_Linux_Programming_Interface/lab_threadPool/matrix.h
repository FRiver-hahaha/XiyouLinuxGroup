#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

using namespace std;

using Matrix = vector<vector<int>>;
// 矩阵乘法
Matrix MatrixMultiply(const Matrix& A, const Matrix& B);

#endif
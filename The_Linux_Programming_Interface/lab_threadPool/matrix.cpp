#include "matrix.h"

Matrix MatrixMultiply(const Matrix& A, const Matrix& B) {
    int m = A.size();// 总数
    int n = B[0].size();// 列数
    int p = B.size();// 总数

    Matrix res(m, vector<int>(n, 0));

    for(int i = 0; i < m; ++i) {
        for(int j = 0; j < n; ++j) {
            for(int k = 0; k < p; ++k) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return res;
}
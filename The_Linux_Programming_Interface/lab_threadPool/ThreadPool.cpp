#include "ThreadPool.h"

using namespace std;

// 阶乘
uint64_t factorial(int n) {
    uint64_t res = 1;
    for(int i = 2; i <= n; ++i) {
        res *= i;
        this_thread::sleep_for(chrono::milliseconds(100));// 模拟耗时长的任务
    }
    return res;
}

using Matrix = vector<vector<int>>;
// 矩阵乘法
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

int main() {
    ThreadPool TPool(10);

    log("(main)测试矩阵乘法:");
    Matrix A = {{1, 2}, {3, 4}};
    Matrix B = {{5, 6}, {7, 8}};

    auto MatrixFuture = TPool.submit([=]() {
        return MatrixMultiply(A, B);
    }, []() {
        log("(callback)矩阵乘法任务已完成");
    });

    Matrix C = MatrixFuture.get();

    log("(main)矩阵结果:");
    for(auto& row : C) {
        for(int x : row) {
            log(to_string(x) + " ", 1);
        }
        log(" ");
    }

    log("(main)测试阶乘:");
    int n;
    log("(main)输入计算的阶乘数字: ");
    cin >> n;
    
    auto factorialFuture = TPool.submit([=]() {
        return factorial(n);
    }, []() {
        log("(callback)阶乘任务已完成");
    });
    log("(main)阶乘结果:");
    auto facResult = factorialFuture.get();
    log("(main)" + to_string(n) + "!= " + to_string(facResult));

    return 0;
}

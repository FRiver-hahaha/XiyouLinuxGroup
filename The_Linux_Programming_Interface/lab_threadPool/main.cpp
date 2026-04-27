#include "ThreadPool.h"
#include "matrix.h"
#include "factorial.h"

using namespace std;

// 阶乘

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

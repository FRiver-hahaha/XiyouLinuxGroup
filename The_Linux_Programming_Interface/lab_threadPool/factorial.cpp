#include "factorial.h"
#include <thread>
#include <chrono>

using namespace std;

uint64_t factorial(int n) {
    uint64_t res = 1;
    for(int i = 2; i <= n; ++i) {
        res *= i;
        this_thread::sleep_for(chrono::milliseconds(100));// 模拟耗时长的任务
    }
    return res;
}
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <algorithm>
#include <chrono>

// ------------------------ 数据生成 ------------------------
std::vector<int> generateRandomData(size_t n, int minVal, int maxVal) {
    std::vector<int> data(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minVal, maxVal);
    for (auto &x : data) x = dis(gen);
    return data;
}

// ------------------------ 归并排序 ------------------------
void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int l = 0; l < temp.size(); ++l) arr[left + l] = temp[l];
}

void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// ------------------------ 多线程归并排序 ------------------------
void threadedMergeSort(std::vector<int>& arr, int left, int right, int depth = 0) {
    const int MAX_DEPTH = 3; // 控制线程创建层数，避免线程过多
    if (left >= right) return;

    if (depth >= MAX_DEPTH) { // 超过最大深度，使用单线程排序
        mergeSort(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    std::thread t1(threadedMergeSort, std::ref(arr), left, mid, depth + 1);
    std::thread t2(threadedMergeSort, std::ref(arr), mid + 1, right, depth + 1);

    t1.join();
    t2.join();

    merge(arr, left, mid, right);
}

// ------------------------ 性能测试 ------------------------
void testPerformance() {
    const size_t DATA_SIZE = 100000;
    std::vector<int> data = generateRandomData(DATA_SIZE, 1, 1000000);
    std::vector<int> dataCopy = data; // 用于单线程排序对比

    // ---- 多线程排序 ----
    auto start = std::chrono::high_resolution_clock::now();
    threadedMergeSort(data, 0, data.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Multi-threaded merge sort time: "
              << std::chrono::duration<double>(end - start).count() << " s\n";

    // ---- 单线程排序 ----
    start = std::chrono::high_resolution_clock::now();
    mergeSort(dataCopy, 0, dataCopy.size() - 1);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-threaded merge sort time: "
              << std::chrono::duration<double>(end - start).count() << " s\n";

    // ---- 验证排序正确性 ----
    if (data == dataCopy) std::cout << "Sorting correct!\n";
    else std::cout << "Sorting ERROR!\n";
}

// ------------------------ 主函数 ------------------------
int main() {
    testPerformance();
    return 0;
}
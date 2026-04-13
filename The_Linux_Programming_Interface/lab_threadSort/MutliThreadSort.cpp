#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;
const int MAX_DEPTH = 3;// 控制线程创建的层数，避免线程创建过多
const int DATA_SIZE = 1e6;
/*

    数据生成：
    创建一个vector，之后使用伪随机数生成器
    通过一个for循环，使用auto，将生成的数据放入ector中

*/

vector<int> createData(size_t size, int minVal, int maxVal) {
    vector<int> a(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minVal, maxVal);// 创建均匀分布，防止数据过密
    for(auto& x : a) {
        x = dis(gen);
    }

    return a;
}

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while(i <= mid && j <= right) {
        if(arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while(i <= mid) temp[k++] = arr[i++];
    while(j <= right) temp[k++] = arr[j++];
    for(int l = 0; l < temp.size(); ++l) {
        arr[left + l] = temp[l];
    }
}

void mSort(vector<int>& arr, int left, int right) {
    if(left >= right) return;
    int mid = left + (right - left) / 2;
    mSort(arr, left, mid);
    mSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void TmSort(vector<int>& arr, int left, int right, int d = 0) {
    if(left >= right) return;

    if(d >= MAX_DEPTH) {// 超过就使用单线程
        mSort(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    // 使用引用，否则无法排序
    thread t1(TmSort, ref(arr), left, mid, d + 1);
    thread t2(TmSort, ref(arr), mid + 1, right, d + 1);
    
    t1.join();
    t2.join();

    merge(arr, left, mid, right);
}

int main() {
    vector<int> data = createData(DATA_SIZE, 1, 100000);
    auto data2 = data;// 单线程数据，进行比较

    // 多线程
    auto startTime = chrono::high_resolution_clock::now();
    TmSort(data, 0, data.size() - 1);
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> temp(endTime - startTime);
    double ThreadTime = temp.count();
    cout << "多线程排序耗时:" << ThreadTime
        << "s" << endl;// 更高精度计算时间


    // 单线程
    startTime = chrono::high_resolution_clock::now();
    mSort(data2, 0, data2.size() - 1);
    endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> temp2(endTime - startTime);
    double SingleTime = temp2.count();
    cout << "单线程排序耗时:" << SingleTime
        << "s" << endl;// 更高精度计算时间

    if(data == data2) {
        cout << "计算正确!" << endl;
        cout << "耗时差距为:" << chrono::duration<double>(abs(temp - temp2)).count() 
            << "s" << endl;
    }else throw domain_error("计算错误!");
    return 0;
}
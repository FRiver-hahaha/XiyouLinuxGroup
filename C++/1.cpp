#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 普通函数

void print01(int val) {
    cout << val << " ";
}   

// 仿函数

class print02 {
public:
    void operator()(int val) {
        cout << val << " ";
    }
};

void test01() {
    vector<int> vec;

    for(int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    for_each(vec.begin(), vec.end(), print01);
    cout << endl;

    for_each(vec.begin(), vec.end(), print02());
}

int main() {
    test01();

    return 0;
}
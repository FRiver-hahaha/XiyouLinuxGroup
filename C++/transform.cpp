#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Transform {
public:
    int operator()(int val) {
        return val;
    }
};

class print01 {
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

    vector<int> vec_target;
    vec_target.resize(vec.size()); // 需要提前开辟空间

    transform(vec.begin(), vec.end(), vec_target.begin(), Transform());
    for_each(vec.begin(), vec.end(), print01());

}   

int main() {
    test01();
    return 0;
}
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void print01(int val) {
    cout << val << " ";
}   

void test01() {
    vector<int> vec;

    for(int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    for_each(vec.begin(), vec.end(), print01);
}

int main() {


    return 0;
}
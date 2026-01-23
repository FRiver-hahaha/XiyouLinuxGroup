#include <iostream>
using namespace std;
const int N = 1e5+10;
int stack[N],top;
/* 单调栈可以维护一个单调数组，具有单调的性质，与栈的结构，先进后出，压栈一致 */

int main() {
    int n;
    cin >> n;
    for(int i = 0; i < n; ++i) {
        int x;
        cin >> x;
        while(top && stack[top] >= x) {
            --top;
        }
        if(top) {
            cout << stack[top] << ' ';
        }else {
            cout << -1 << ' ';
        }
        stack[++top] = x;
    }
    return 0;
}
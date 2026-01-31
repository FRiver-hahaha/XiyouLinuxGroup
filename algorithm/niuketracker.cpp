#include <iostream>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;


int main() {
    ios:: sync_with_stdio(false);// 去除缓冲区不同步
    cin.tie(nullptr);// 关闭 cin 和 cout 的绑定
    LL ar[3];
    for(int i = 0; i < 3; ++i) {
        cin >> ar[i];
    }

    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3 - i - 1; ++j) {
            if(ar[j] > ar[j + 1]) {
                LL tmp = ar[j];
                ar[j] = ar[j + 1];
                ar[j + 1] = tmp;
            }
        }
    }

    cout << "The maximum number is : " << ar[2] << endl;
    cout << "The minimum number is : " << ar[0] << endl;

    return 0;
}
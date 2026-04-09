#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 10;

int prime[N], cnt = 0;// prime[] 存储所有素数

bool f[N];// 存储i是否被筛掉

void get_prime(int n) {
    for(int i = 2; i <= n; ++i) {
        if(f[i]) continue;
        prime[++cnt] = i;// 将当前素数i记录到prime数组中

        for(int j = i + i; j <= n; j += i) {
            f[j] = true;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int n;
    cin >> n;
    get_prime(n);

    cout << cnt << endl;

    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int n, k;
int a[100010];
long long sum[100010];
long long cnt[100010];
long long res;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> k;

    for(int i = 1; i <= n; ++i) {
        cin >> a[i];
        sum[i] = a[i] + sum[i - 1];
    }

    cnt[0] = 1;

    for(int i = 1; i <= n; ++i) {
        res += cnt[sum[i] % k];
        cnt[sum[i] % k]++;
    }

    cout << res << endl;
    
    return 0;
}
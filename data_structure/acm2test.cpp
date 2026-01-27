#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;

LL T, n, m, ans = 0;
LL f[maxn] = {};

int main() {
    scanf("%lld",&n);
    f[1] = f[2] = 1;
    for(int i = 3; i <= n; ++i) {
        f[i] = (f[i - 1] + f[i - 2]) % m;
    }

    printf("%lld", f[n]);

    return 0;
}

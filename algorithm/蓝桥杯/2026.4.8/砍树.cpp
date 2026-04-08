#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int N = 1e6 + 10;
ll a[N];
ll maxn = 0, n, m;

int check(ll x) {
    ll res = 0;
    for(int i = 1; i <= n; ++i) {
        if(a[i] >= x) res += a[i] - x;
    }

    return res >= m;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;

    for(int i = 1; i <= n; ++i) {
        cin >> a[i];
        maxn = max(maxn, a[i]);
    }

    ll l = 1, r = maxn;
    while(r - l > 1) {
        ll mid = (l + r) >> 1;
        if(check(mid)) l = mid;
        else r = mid;
    }

    if(check(r)) cout << r;
    else cout << l;

    return 0;
}
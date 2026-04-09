#include <bits/stdc++.h>
using namespace std;

int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    vector<int> a(n);
    for(int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    sort(a.begin(), a.end());

    int d = 0;
    for(int i = 1; i < n; ++i) {
        d = gcd(d, a[i] - a[0]);
    }

    if(d == 0) {
        cout << n << endl;
    }else {
        cout << (a[n - 1] - a[0]) / d + 1 << endl;
    }

    return 0;
}
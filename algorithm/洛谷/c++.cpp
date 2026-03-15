#include <bits/stdc++.h>
#include <cstring>
using namespace std;
#define LL long long


int main() {
    LL n, m;
    LL level = LLONG_MAX;

    cin >> n >> m;

    vector<LL> A(n);
    vector<LL> B(n);

    for(int i = 0; i < n; ++i) {
        cin >> A[i];
        B[i] = A[i] * A[i];
    }

    sort(B.begin(), B.end());

    for(int i = 0; i + m - 1 < n; ++i) {
        level = min(level, B[i + m - 1] - B[i]);
    }

    cout << level << endl;

    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int a[9] = {1, 2, 3, 4, 5, 6, 7, 8 ,9};
int b, c, d, e;
int ans = 0;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    while(next_permutation(a, a + 9)) {
        b = a[0] * a[2] * (a[6] * 100 + a[7] * 10 + a[8]);
        c = a[1] * (a[6] * 100 + a[7] * 10 + a[8]);
        d = (a[3] * 100 + a[4] * 10 + a[5]) * a[2];
        e = 10 * a[2] * (a[6] * 100 + a[7] * 10 + a[8]);
        if(b + c + d == e) ans++;
    }
    cout << ans;

    return 0;
}
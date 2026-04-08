#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int a[6] = {20, 10, 50, 20, 20, 40};
    sort(a, a + 6);
    int *p = lower_bound(a + 0, a + 5 + 1, 20);

    return 0;
}
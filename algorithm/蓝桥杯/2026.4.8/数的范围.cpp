#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 10;

int a[N];
int n, q;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n >> q;
    for(int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    while(q--) {
        int x;
        cin >> x;

        int l = 1, r = n;
        int ans;

        while(l <= r) {
            int mid = l + (r - l) / 2;
            if(a[mid] >= x) {
                ans = mid;
                r = mid - 1;
            }else {
                l = mid + 1;
            }
        }

        if(a[ans] != x) {
            cout << "-1 -1" << endl;
            continue;
        }

        cout << ans - 1 << ' ';

        l = 1, r = n;
        ans = 1;

        while(l <= r) {
            int mid = l + (r - l) / 2;
            if(a[mid] <= x) {
                ans = mid;
                l = mid + 1;
            }else {
                r = mid - 1;
            }
        }

        cout << ans - 1 << endl;

    }


    return 0;
}
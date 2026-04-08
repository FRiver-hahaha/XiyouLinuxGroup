#include <bits/stdc++.h>
using namespace std;
const int N = 100010;
int a[N], st[N];// st:标记函数,可以用哈系表代替
int n;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n; 
    for(int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    int res = 1;
    for(int i = 0, j = 0; i < n; ++i) {
        st[a[i]]++;
        while(j <= i && st[a[i]] > 1) {
            st[a[j]]--;
            j++;
        }
        res = max(res, i - j + 1);
    }
    cout << res << endl;
    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int m = 20, n = 20, num = 0;
    int cnt = m + n - 1;
    for(int i = 1; i < cnt; ++i) {
        num += i;
    }

    if(cnt % 2 == 0) {
        int row = 1;
        while(row <= m) {
            num++;
            row++;
        }
    }else {
        int row = cnt;
        while(row >= m) {
            num++;
            row--;
        }
    }

    cout << num << endl;

    return 0;
}
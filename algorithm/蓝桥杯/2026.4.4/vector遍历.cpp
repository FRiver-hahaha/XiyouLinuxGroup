#include <bits/stdc++.h>
using namespace std;
vector<int> a[100100];
int n, m, i, j, x, y;

int main() {
    ios::sync_with_stdio(false);
    cin >> n >> m;
    for(i = 1; i <= m; ++i) {
        cin >> x >> y;
        a[x - 1].push_back(y);
    }

    for(i = 0; i < n; ++i) {
        cout << a[i].size();
        sort(a[i].begin(), a[i].end());

        for(j = 0; j < a[i].size(); ++j) {
            cout << " " << a[i][j];
        }
        cout << endl;
    }

    return 0;
}
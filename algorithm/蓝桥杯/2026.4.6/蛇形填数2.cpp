#include <bits/stdc++.h>
using namespace std;

int a[110][110];

int main() {
    int tot, m, n, x, y;
    cin >> m >> n;
    x = 0;
    memset(a, 0, sizeof(a));
    y = n - 1;
    a[x][y] = 1;
    tot = 1;
    while(tot < m * n) {
        while(x + 1 < m && !a[x + 1][y]) a[++x][y] = ++tot;
        while(y - 1 >= 0 && !a[x][y - 1]) a[x][--y] = ++tot;
        while(x - 1 >= 0 && !a[x - 1][y]) a[--x][y] = ++tot;
        while(y + 1 >= 0 && !a[x][y + 1]) a[x][++y] = ++tot;
    }

    for(int i = 0; i < m; ++i) {
        for(int j = 0; j < n; ++j) {
            printf("%5d", a[i][j]);
        } 
        printf("\n");

    }
    printf("\n");

    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int n;
char a[1010][1010];// 地图
int vis[1010][1010] = {0};// 标记是否被搜索过
int d[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};// 四个方向
int flag;// 用于标记这个岛屿是否被淹没

void dfs(int x, int y) {
    vis[x][y] = 1;// 标记这个"#"被搜索过
    if(a[x - 1][y] == '#' && 
        a[x + 1][y] == '#' && 
        a[x][y - 1] == '#' &&
        a[x][y + 1] == '#') {
            flag = 1;// 上下左右都是陆地，不会被淹没
        }
    for(int i = 0; i < 4; ++i) {// 继续dfs周围的陆地
        int nx = x + d[i][0];
        int ny = y + d[i][1];
        if(vis[nx][ny] == 0 && a[nx][ny] == '#') {
            dfs(nx, ny);
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n;
    for(int i = 1; i <= n; ++i) {
        for(int j = 1; j <= n; ++j) {
            cin >> a[i][j];
        }
    }
    int ans = 0;

    for(int i = 1; i <= n; ++i) {// dfs所有像素点
        for(int j = 1; j <= n; ++j) {
            if(a[i][j] == '#' && vis[i][j] == 0) {
                flag = 0;
                dfs(i, j);
                if(flag == 0) {// 这个岛全部被淹没
                    ans++;// 统计岛屿的数量
                }
            }
        }
    }

    cout << ans << endl;

    return 0;
}
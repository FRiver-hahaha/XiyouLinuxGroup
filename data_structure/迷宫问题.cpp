#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 11000, M = 1000000007, inf = 1LL << 62;
LL n, m, sx, sy, tx, ty, ans = 0;
const LL dx[4] = {-1, 0, 0, 1};// 行方向数组，四连通遍历当前坐标的周围
const LL dy[4] = {0, -1, 1, 0};// 列方向数组，四连通遍历当前坐标的周围
char ch[maxn][maxn] = {};// 地图结构
LL dep[maxn][maxn] = {};// 从坐标(sx, sy)走到(i, j)的最小步数
bool vis[maxn][maxn] = {};// (i, j)是否已经被遍历
queue<pair<LL, LL> >q;// bfs队列

bool ok(LL x, LL y) {// 判断是否入队
    if(!(x >= 1 && x <= n && y >= 1 && y <= m )) return 0;// 判断是否为坐标
    if(ch[x][y] == '*') return 0;// 判断是否为墙壁
    if(vis[x][y]) return 0;// 判断是否被记录
    return 1;
}

void bfs() {// 从S点开始的bfs
    q.push(make_pair(sx, sy));// 把根节点扔到队尾
    dep[sx][sy] = 0;
    vis[sx][sy] = 1;// 记录状态
    while(!q.empty()) {// 开始队列操作
        LL x = q.front().first;// 访问队首状态
        LL y = q.front().second;
        q.pop();

        for(LL i = 0; i < 4; ++i) {
            LL xx = x + dx[i];
            LL yy = y + dy[i];
            if(!ok(xx, yy)) continue;
            q.push(make_pair(xx, yy));// 加入队列
            vis[xx][yy] = 1;
            dep[xx][yy] = dep[x][y] + 1;
        }
    }
}

int main() {

    scanf("%lld %lld", &n, &m);
    scanf("%lld %lld %lld %lld",&sx, &sy, &tx, &ty);
    for(LL i = 1; i <= n; ++i) {
        scanf("%s", ch[i] + 1);
    }
    for(LL i = 1; i <= n; ++i) {
        for(LL j = 1; j <= m; ++j) {
            dep[i][j] = inf;
        }
    }// 步数初始化为正inf

    bfs();
    // for(LL i = 1; i <= n; ++i) {
    //     for(LL j = 1; j <= m; ++j) {
    //         LL tmp = dep[i][j];
    //         if(tmp == inf) {
    //             printf("xx ");
    //         }else {
    //             printf("%2lld ",tmp);
    //         }
    //     } 
    //     printf("\n");
    // }
        
    ans = dep[tx][ty];
    if(ans == inf) {
        printf("-1");
    }else {
        printf("%lld", ans);
    }

    return 0;

}
/* 
牛客：HIGH43 走迷宫
*/
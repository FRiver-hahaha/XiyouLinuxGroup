#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;

LL T, n, m, ans = 0;
LL lin[maxn] = {}, len = 0;

bool vis[maxn] = {};// 标记哪一个点被访问过了
struct edge {
    LL y, nxt;
}e[maxn];

void add(LL x, LL y) {
    e[++len].nxt = lin[x];
    lin[x] = len;
    e[len].y = y;
}// 生成图

void dfs(LL x) {// x代表了当前访问了哪一个节点 
    printf(":%lld\n", x);
    vis[x] = 1;

    for(LL i = lin[x]; i; i = e[i].nxt) {// 遍历x的所有相邻节点
        LL y = e[i].y;
        if(vis[y]) continue;
        dfs(y);
    }
}

int main() {
    scanf("%lld", &n);
    for(LL i = 1; i < n; ++i) {// 生成图
        LL x, y;
        scanf("%lld %lld", &x, &y);
        add(x, y);
        add(y, x);
    }
    dfs(1);// 深度优先搜索
    return 0;
}
/* 
7 // 节点数
1 2 // 当前节点 当前节点的相邻节点
1 3
3 6
3 7
2 4
2 5
*/
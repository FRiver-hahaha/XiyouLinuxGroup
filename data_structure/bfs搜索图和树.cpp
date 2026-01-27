#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;
LL n;
LL lin[maxn] = {}, len = 0;
LL dep[maxn] = {};// 记录距离(深度)
bool vis[maxn] = {};// 标记哪一个点被访问过了
queue< LL >q;// bfs队列
struct edge {
    LL y, nxt;
}e[maxn];

void add(LL x, LL y) {
    e[++len].nxt = lin[x];
    lin[x] = len;
    e[len].y = y;
}// 生成图

void bfs(LL S) {// 从S点开始的bfs
    q.push(S);// 把根节点扔到队尾
    dep[S] = 0;
    vis[S] = 1;// 记录状态
    while(!q.empty()) {// 开始队列操作
        LL x = q.front();// 取出队首元素
        for(LL i = lin[x]; i; i = e[i].nxt) {// 遍历相邻元素
            LL y = e[i].y;
            if(vis[y]) continue;
            q.push(y);
            dep[y] = dep[x] + 1;// 深度 + 1
            vis[y] = 1;// 记录状态
        }
        q.pop();// 弹出节点x，防止死循环
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
    bfs(1);// 广度优先搜索
    for(LL i = 1; i <= n ; ++i) {
        printf("%lld的深度为：%lld\n",i, dep[i]);
    }
    return 0;

}
/* 
// 节点数
// 当前节点 当前节点的相邻节点
7 
1 2 
1 3
3 6
3 7
2 4
2 5
*/
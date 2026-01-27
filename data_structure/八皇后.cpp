#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;
LL n, a[maxn] = {};// a数组代表目前全排列出现的情况
bool vis[maxn] = {}; // 状态数组,代表哪些列已经被其他的皇后占据了
LL ans = 0;
unordered_map<LL, bool>s, t;

void dfs(LL dep) {// 当前dfs的深度，即该填入的第几行的皇后
    
    if(dep > n) {
        ans++;
        if(ans <= 3) {
            for(LL i = 1;i <= n; ++i) {
                printf("%lld ",a[i]);
                
            }
            printf("\n");
        }
        
        return;
    }
    
    for(LL i = 1; i <= n; ++i) {

        // 剪枝
        if(vis[i]) continue;//列
        if(s[dep - i]) continue;// 主对角线x - y = c
        if(t[dep + i]) continue;// 副对角线x + y = c

        s[dep - i] = t[dep + i] = vis[i] = 1;// 确定数组状态，已经被填写
        a[dep] = i;// 第dep行的皇后在第i列
        dfs(dep + 1);  
        a[dep] = 0;
        s[dep - i] = t[dep + i] = vis[i] = 0;// 还原数组状态
    }
}

int main() {
    scanf("%lld", &n);
    dfs(1);
    printf("%lld",ans);
    return 0;
}
/*
洛谷p1219
*/
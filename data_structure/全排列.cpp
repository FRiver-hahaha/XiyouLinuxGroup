#include <bits/stdc++.h>
using namespace std;
#define LL long long
const LL maxn = 2200000, M = 1000000007, inf = 1LL << 62;
LL n, a[maxn] = {};// a数组代表目前全排列出现的情况
bool vis[maxn] = {}; // 状态数组

void dfs(LL dep) {// 当前dfs的深度，即该填入的第几个数字
    
    if(dep > n) {
        for(LL i = 1;i <= n; ++i) {
            printf("%5lld",a[i]);
            
        }
        printf("\n");
        return;
    }
    
    for(LL i = 1; i <= n; ++i) {
        if(vis[i]) continue;
        vis[i] = 1;// 确定数组状态，已经被填写
        a[dep] = i;// 第dep 个数被填写成了i
        dfs(dep + 1);
        vis[i] = 0;// 还原数组状态
    }
}

int main() {
    scanf("%lld", &n);
    dfs(1);
    return 0;
}
/*
123
132
213
231
312
321
*/
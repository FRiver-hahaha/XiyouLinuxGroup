#include <bits/stdc++.h>
using namespace std;

int n, a, b;
int x[205], step[205];// 数组x用于存储每个位置可以跳跃的步数，step数组用于标记每个位置的步数
queue<int> q;// 队列用于bfs

void bfs() {
    q.push(a);// 将起点a加入队列
    step[a] = 0;// 将起点的步数标记为0
    while(!q.empty()) {// 当队列不为空时
        int t = q.front();// 取出队首元素
        q.pop();// 弹出队首元素
        if(t == b) {// 如果当前位置等于终点b
            cout << step[b];// 输出步数
            return;// 结束搜索
        }
        if(t - x[t] <= n && !step[t - x[t]]) {// 如果向下跳不越界，并下一个位置没有被访问过
            q.push(t - x[t]);// 将下一个位置加入队列
            step[t - x[t]] = step[t] + 1;// 更新下一个位置的步数
        }

        if(t + x[t] <= n && !step[t + x[t]]) {// 如果向上跳不越界，并下一个位置没有被访问过
            q.push(t + x[t]);// 将下一个位置加入队列
            step[t + x[t]] = step[t] + 1;// 更新下一个位置的步数
        }
    }

    cout << -1; // 如果无法到达终点b，则返回-1
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n >> a >> b;
    for(int i = 1; i <= n; ++i) {// 输入每个位置可以跳跃的步数
        cin >> x[i];
    }
    bfs();

    return 0;
}
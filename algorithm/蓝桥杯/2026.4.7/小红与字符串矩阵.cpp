#include <bits/stdc++.h>
using namespace std;

int n, m, ans;// ans记录满足条件的数量

const int N = 1005;
char a[N][N];// 存储地图的二维数组
int dx[] = {-1, 1, 0, 0};// 方向数组
int dy[] = {0, 0, -1, 1};// 方向数组

string tencent = "tencent";// 目标字符串

struct point {// 定义结构体表示点的坐标以及当前字符串
    int x, y;// 当前坐标
    string s;// 当前字符串
};

void bfs(int x, int y) {
    queue<point> q;// 用于存储待访问的点
    string tmp = "";// 初始化当前字符串为空字符串
    tmp.push_back(a[x][y]);// 将起点的字符加入当前字符串
    q.push(point{x, y, tmp});// 将起点加入队列

    while(q.size()) {// 当队列不为空的时候
        point p = q.front();// 取出队首元素
        q.pop();// 取出队首元素
        string str = p.s; // 取出当前字符串
        if(str == "tencent") {
            ans++;
        }
        if(str.size() == 7) {
            continue;
        }
        int flag = 0;// 用于判断当前字符串与目标字符串是否不同

        for(int i = 0; i < str.size(); ++i) {// 遍历当前字符串
            if(str[i] != tencent[i]) {
                flag = 1;
                break;
            }
        }

        if(flag) {
            continue;
        }

        int x = p.x;// 取出当前点的坐标
        int y = p.y;

        for(int i = 0; i < 4; ++i) {
            int xx = x + dx[i];// 计算下一个点的横坐标
            int yy = y + dy[i];// 计算下一个点的纵坐标
            if(xx >= 1 && xx <= n && yy >= 1 && yy <= m) {
                q.push(point{xx, yy, str + a[xx][yy]});// 将下一个点加入队列，并且更新当前字符串
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n >> m;

    for(int i = 1; i <= n; ++i) {
        for(int j = 1; j <= m; ++j) {
            cin >> a[i][j];
        }
    }

    for(int i = 1; i <= n; ++i) {
        for(int j = 1; j <= n; ++j) {
            bfs(i, j);// 从当前点开始bfs
        }
    }

    cout << ans << endl;

    return 0;
}
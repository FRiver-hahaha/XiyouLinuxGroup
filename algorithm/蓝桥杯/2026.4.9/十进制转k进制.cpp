#include <bits/stdc++.h>
using namespace std;

long long s, base;

string p = "0123456789ABCDEF";

string ans;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> s >> base;

    while(s) {
        ans.push_back(p[s % base]);// 将s对base取模的结果作为索引，
        //    将对应的字符添加到ans末尾
        s /= base;
    }   

    reverse(ans.begin(), ans.end());

    cout << ans << endl;

    return 0;
}
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    string s;
    int cnt = 1;
    cin >> s;

    for(int i = 0; i < s.size() - 1; ++i) {
        if(s[i] == s[i + 1]) cnt++;
        else {
            cout << s[i] << cnt;
            cnt = 1;
        }
    }
    cout << s[s.size() - 1] << cnt;

    return 0;
}
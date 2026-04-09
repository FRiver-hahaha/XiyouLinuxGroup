#include <bits/stdc++.h>
using namespace std;

int main() {
    string s;

    long long base = 0, ans = 0, k = 0;

    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> s >> base;

    for(int i = s.size() - 1; i >= 0; --i) {
        if(s[i] >= 'A') {
            ans += (s[i] - 'A' + 10) * pow(base, k++);
        }else {
            ans += (s[i] - '0') * pow(base, k++);
        }
    }

    cout << ans << endl;

    return 0;
}
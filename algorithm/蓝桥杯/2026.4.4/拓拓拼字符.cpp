#include <bits/stdc++.h>
using namespace std;

string s[100];

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n, l;
    cin >> n >> l;
    for(int i = 0; i < n; ++i) cin >> s[i];
    sort(s, s + n);
    for(int i = 0; i < n; ++i) cout << s[i];
    cout << endl;

    return 0;
}
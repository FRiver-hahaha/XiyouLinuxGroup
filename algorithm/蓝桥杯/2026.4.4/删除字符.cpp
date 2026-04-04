#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    string s;
    char ch;
    getline(cin, s);
    cin >> ch;

    int p = s.find(ch);

    while(p != string::npos) {
        s.erase(p, 1);
        p = s.find(ch);
    }

    cout << s;

    return 0;
}
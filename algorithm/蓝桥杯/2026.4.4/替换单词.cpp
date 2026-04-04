#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    string s, w, t;
    int p;

    getline(cin, s);
    w = "black";
    t = "block";
    p = s.find(w);

    while(p != string::npos) {
        s.replace(p, 5, t);
        p = s.find(w);
    }

    cout << s;

    return 0;
}
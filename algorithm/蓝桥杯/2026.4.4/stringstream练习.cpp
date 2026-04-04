#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    string s;
    stringstream ss;
    int n, sum, a;

    cin >> n;
    getline(cin, s);
    for(int i = 0; i < n; ++i) {
        getline(cin , s);
        ss.clear();
        ss.str(s);
        sum = 0;
        while(1) {
            ss >> a;
            if(ss.fail()) break;
            sum += a;
        }

        cout << sum << endl;
    }

    return 0;
}
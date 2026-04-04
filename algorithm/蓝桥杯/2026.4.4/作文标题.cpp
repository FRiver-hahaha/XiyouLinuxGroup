#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    string str;
    int cnt = 0;
    getline(cin, str);

    for(int i = 0; i < str.size(); i++) {
        if(str[i] != ' ') {
            cnt++;
        }
    }

    cout << cnt << endl;

    return 0;
}
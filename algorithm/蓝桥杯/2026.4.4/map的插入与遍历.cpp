#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n, age;
    string s;
    map<string, int> mp;
    cin >> n;
    for(int i = 0; i < n; i++) {
        cin >> s >> age;
        mp[s] = age;
    }

    map<string, int>::iterator it;
    for(it = mp.begin(); it != mp.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }

    return 0;
}
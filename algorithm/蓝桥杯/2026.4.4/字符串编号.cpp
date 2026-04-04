#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    string s;
    cin >> s;
    string result = "";
    int n = s.size();
    int i = 0;

    while(i < n) {
        if(i + 1 < n) {
            int twoDigit = (s[i] - '0') * 10 + (s[i + 1] - '0');
            if(twoDigit >= 10 && twoDigit <= 26) {
                i += 2;
                result += (char)('A' + twoDigit - 1);
                continue;
            }
        }

        int oneDigit = s[i]  - '0';
        result += (char)('A' + oneDigit - 1);
        i++;
    }

    cout << result << endl;

    return 0;
}
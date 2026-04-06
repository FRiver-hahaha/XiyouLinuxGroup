#include <bits/stdc++.h>
using namespace std;

bool check(int x) {
    while(x > 0) {
        if(x % 10 <= 2 || x % 10 == 9) {
            return true;
        }else {
            x /= 10;
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n;
    long long sum = 0;
    cin >> n;
    for(int i = 1; i <= n; ++i) {
        if(check(i)) {
            sum += i;
        }
    }


    cout << sum << endl;
    return 0;
}
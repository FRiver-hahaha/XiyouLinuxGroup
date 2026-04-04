#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int n;
    int count = 0;
    int sum = 0;
    while(cin >> n) {
        if(n == 0) break;

        if(n % 2 == 0) {
            count++;
        }else {
            sum += n;
        }
    }

    cout << count << endl << sum;

    return 0;
}
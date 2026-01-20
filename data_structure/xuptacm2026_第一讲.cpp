// // A - 求区间和(前缀和)

// #include <iostream>
// #include <vector>
// using namespace std;

// int main() {
//     int n, m;
//     cin >> n;
//     vector<int> ar(n + 1);
//     vector<long long> sum(n + 1);
//     for(int i = 1; i <= n; ++i) {
//         cin >> ar[i];
//     }
//     cin >> m;
//     sum[0] = 0;
//     for(int i = 1; i <= n; ++i) {
//         sum[i] = sum[i - 1] + ar[i];
//     }

//     for(int i = 0; i < m; ++i) {
//         int l, r;   
//         cin >> l >> r;
//         cout << sum[r] - sum[l - 1] << endl;
//     }

//     return 0;
// }

// B - 语文成绩(差分)

#include <iostream>
using namespace std;
int ar[5000001];
int sum[5000001];

int main() {
    int n, p;
    cin >> n >> p;
    
    for(int i = 1; i <= n; ++i) {
        cin >> ar[i];
        sum[i] = ar[i] - ar[i - 1];
    }

    for(int i = 0; i < p; ++i) {
        int x, y, z;
        cin >> x >> y >> z;
        sum[x] += z;
        sum[y + 1] -= z;
    }

    for(int i = 1; i <= n; ++i) {
        ar[i] = ar[i - 1] + sum[i];
    }
    
    int min = 1;

    for(int i = 2; i <= n; ++i) {
        if(ar[i] < ar[min]) {
            min = i;
        }
    }

    cout << ar[min] << endl;

    return 0;
}
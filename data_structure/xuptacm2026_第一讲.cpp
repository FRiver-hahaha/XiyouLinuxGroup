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

// // B - 语文成绩(差分)

// #include <iostream>
// using namespace std;
// int ar[5000001];
// int sum[5000001];

// int main() {
//     int n, p;
//     cin >> n >> p;
    
//     for(int i = 1; i <= n; ++i) {
//         cin >> ar[i];
//         sum[i] = ar[i] - ar[i - 1];
//     }

//     for(int i = 0; i < p; ++i) {
//         int x, y, z;
//         cin >> x >> y >> z;
//         sum[x] += z;
//         sum[y + 1] -= z;
//     }

//     for(int i = 1; i <= n; ++i) {
//         ar[i] = ar[i - 1] + sum[i];
//     }
    
//     int min = 1;

//     for(int i = 2; i <= n; ++i) {
//         if(ar[i] < ar[min]) {
//             min = i;
//         }
//     }

//     cout << ar[min] << endl;

//     return 0;
// }

// // C - 光骓者的荣耀
// #include <iostream>
// using namespace std;
// const long long N = 1e3 + 10;
// long long arr[N];

// int main() {
//     int n,k;
//     cin >> n >> k;
//     long long max = 0;
//     long long sum = 0;
//     for(int i = 0; i < n - 1; ++i) {
//         cin >> arr[i];
//         sum += arr[i];
//         if(arr[i] > max) {
//             max = arr[i];
//         }
//     }
//     int tt = 1;
//     long long sum_t = 0;
//     if(k) {
//         for(int i = 0; i < n - 1; ++i) {
//             if(tt && arr[i + k - 1] == max) {
//                tt = 0;
//                i += k; 
//             }else {
//                 sum_t += arr[i];
//             }
//         }
//         cout << sum_t << endl;
//     }else {
//         cout << sum << endl;
//     }
//     return 0;
// }

// // D - Tail of Snake(前缀和)
// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <climits>
// using namespace std;

// int main() {
//     ios::sync_with_stdio(false);
//     cin.tie(nullptr);
    
//     int N;
//     cin >> N;
    
//     vector<long long> A(N+1), B(N+1), C(N+1);
//     vector<long long> PA(N+1, 0), PB(N+1, 0), PC(N+1, 0);
    
//     // 读取数组并计算前缀和
//     for (int i = 1; i <= N; i++) {
//         cin >> A[i];
//         PA[i] = PA[i-1] + A[i];
//     }
//     for (int i = 1; i <= N; i++) {
//         cin >> B[i];
//         PB[i] = PB[i-1] + B[i];
//     }
//     for (int i = 1; i <= N; i++) {
//         cin >> C[i];
//         PC[i] = PC[i-1] + C[i];
//     }
    
//     // 预处理 diffA 和 diffB
//     vector<long long> diffA(N+1), diffB(N+1);
//     for (int i = 1; i <= N; i++) {
//         diffA[i] = PA[i] - PB[i];
//         diffB[i] = PB[i] - PC[i];
//     }
    
//     long long maxDiffA = LLONG_MIN;
//     long long answer = LLONG_MIN;
    
//     // 遍历所有可能的 y (2 ≤ y ≤ N-1)
//     for (int y = 2; y < N; y++) {
//         // 更新到 y-1 为止的最大 diffA
//         maxDiffA = max(maxDiffA, diffA[y-1]);
        
//         // 计算当前 (x,y) 对的值
//         long long current = maxDiffA + diffB[y] + PC[N];
//         answer = max(answer, current);
//     }
    
//     cout << answer << endl;
    
//     return 0;
// }

// // E - 地毯(前缀和 + 差分)

// #include <iostream>
// #include <vector>
// using namespace std;

// int main() {
//     int n, m;
//     cin >> n >> m;
//     int i, j;
//     vector<vector <int>> diff(n + 2, vector<int>(n + 2, 0));
//     vector<vector <int>> ans(n + 2, vector<int>(n + 2, 0));

//     for(i = 0; i < m; ++i) {
//         int x1, y1, x2, y2;
//         cin >> x1 >> y1 >> x2 >> y2;
//         diff[x1][y1] += 1;
//         diff[x1][y2 + 1] -= 1;
//         diff[x2 + 1][y1] -= 1;
//         diff[x2 + 1][y2 + 1] += 1;
//     }

//     for(i = 1; i <= n; ++i) {
//         for(j = 1; j <= n; ++j) {
//             ans[i][j] = diff[i][j] + ans[i - 1][j] + ans[i][j - 1] - ans[i - 1][j - 1];
//         }
//     }

//     for(i = 1; i <= n; ++i) {
//         for(j = 1;j <= n; ++j) {
//             cout << ans[i][j] << ' ';
//         }
//         cout << endl;
//     }

//     return 0;
// }

// // F - 求和(前缀和)

// #include <iostream>
// using namespace std;
// const int N = 1e6 + 10;
// long long arr[N];
// long long sum[N];


// int main() {
//     int n, i;
//     cin >> n;
//     for(i = 1; i <= n; ++i) {
//         cin >> arr[i];
//         sum[i] = arr[i] + sum[i - 1];
//     }
//     long long x = 0;
//     for(i = 1; i <= n; ++i) {
//         x += (sum[n] - sum[i]) * arr[i];
//     }

//     cout << x << endl;


//     return 0;
// }

// // G - 查找(二分)
// #include <iostream>
// using namespace std;
// const int N = 1e6+10;
// long long ar[N];

// int main() {
//     int n, m;
//     long long left, right, middle;
//     long long x;
//     cin >> n >> m;
//     for(int i = 0; i < n; ++i) {
//         cin >> ar[i];
//     }

//     for(int i = 0; i < m; ++i) {
//         int tt = -1;
//         cin >> x;
//         left = 0;
//         right = n - 1;
//         middle = left + (right - left) / 2;
//         while(left <= right) {
//             middle = left + (right - left) / 2;
//             if(x > ar[middle]) {
//                 left = middle + 1;
//             }else if(x < ar[middle]) {
//                 right = middle - 1;
//             }
//             else {
//                 right = middle - 1;
//                 tt = middle + 1;
//             }
            
//         }
//         cout << tt << " ";
//     }

//     return 0;
// }

// I - 木材加工()
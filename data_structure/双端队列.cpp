#include <iostream>
#include <deque>
using namespace std;
const int N = 1e6 + 10;
int a[N];
deque<int> q;

void solve() {
    int n,k;
    cin >> n >> k;
    for(int i = 1; i < n; ++i) {
        cin >> a[i];
    }
    for(int i = 1; i < n; ++i) {
        while(q.size() && a[q.back()] > a[i]) {
            q.pop_back();
        }
        q.push_back(i);
        if(i >= k) {
            cout << a[q.front()] << " ";
        }
        if(q.size() && i = q.front() == k - 1) {
            q.pop_front();
        }
    }
    cout << endl;
    q.clear();
    for(int i = 1; i <= n; ++i) {
        while(q.size() && a[q.back()] < a[i]) {
            q.pop_back();
        }
        q.push_back(i);
        if(i >= k) {
            cout << a[q.front()] << " ";
        }
        if(q.size() && i - q.front() == k - 1) {
            q.pop_front();
        }
    }

}

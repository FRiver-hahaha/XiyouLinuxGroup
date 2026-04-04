#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    set<int> st;
    int n, m;
    cin >> n;

    for(int i = 0; i < n; ++i) {
        cin >> m;
        st.insert(m);
    }

    set<int>::iterator it;

    for(it = st.begin(); it != st.end(); it++) {
        cout << *it << " ";
    }

    return 0;
}
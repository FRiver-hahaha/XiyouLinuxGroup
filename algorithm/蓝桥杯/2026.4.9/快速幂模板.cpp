#include <bits/stdc++.h>
using namespace std;

#define ll long long;

ll qpow(ll a, ll b, ll q) {// 求a的b次方对q取模的值
    ll res = 1 % q;

    while(b) {// 当指数不为0
        if(b % 2 == 1) {
            res = (res % p) * (a % p) % p;
        }
        a = (a % p) * (a % p) % p;// 底数平方
        b /= 2;// 指数除2
    }
    return res;
}

int main() {


    return 0;
}
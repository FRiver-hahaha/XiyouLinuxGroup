#include <iostream>
#include <cstring>
using namespace std;
#define LL long long

int main() {
    LL n;
    char name[20];
    scanf("%lld", &n);
    LL sum = 0, money = 0;
    for(int i = 0; i < n; ++i) {
        scanf("%s %lld", name, &money);
        if(strcmp(name, "Q") == 0) {
            sum += money;
        }else {
            continue;
        }
    }
    printf("%lld", sum);

    return 0;
}
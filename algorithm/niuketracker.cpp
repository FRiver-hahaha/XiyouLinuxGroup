#include <stdio.h>
#include <string.h>

int main() {
    const int N = 1e6;
    char ch[N];
    scanf("%s",ch);
    long long len = strlen(ch);
    if(ch[1] >= '0' + 5) {
        ch[0] += 1;
        ch[1] = '0';
    }
    printf("%c.%c*10^%lld", ch[0], ch[1], len - 1);
}
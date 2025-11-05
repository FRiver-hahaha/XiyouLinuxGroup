// 3

#include<stdio.h>
#include<limits.h>

int bitSearch(char*,int);

int main(void) {
    int n;
    scanf("%d",&n);
    char bitStr[CHAR_BIT * sizeof(int) + 1];
    printf("%d",bitSearch(bitStr,n));
    return 0;
}

int bitSearch(char* str,int n) {
    int i;
    int cnt = 0;
    const static int size = CHAR_BIT * sizeof(int);
    for(i = size - 1; i >= 0; i--, n >>= 1) {
        str[i] = (01 & n) + '0';
        if(01 & n) {
            cnt++;
        }
    }
    str[size] = '\0';
    return cnt;
}
// 3

#include<stdio.h>

int main() {
    char ch;
    int cnt_l = 0,cnt_s = 0;
    while( scanf("%c",&ch) != EOF ) {
        if( ch >= 65 && ch <= 65+26 ) {
            cnt_l++;
        }
        if( ch >= 97 && ch <= 97+26 ) {
            cnt_s++;
        }
    }
    printf("cnt_l=%d cnt_s=%d",cnt_l,cnt_s);

    return 0;
}
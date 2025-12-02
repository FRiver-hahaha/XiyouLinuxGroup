// 1

#include<stdio.h>

int main() {
    char ch;
    int cnt = 0;
    while( scanf("%c",&ch) != EOF ) {
        cnt++;
    }
    printf("%d",cnt);

    return 0;
}
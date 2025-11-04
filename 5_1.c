// 2

#include<stdio.h>

int main() {
    int n;
    scanf("%d",&n);
    for( int i = 5; i <= n+10; i++) {
        printf("%d",i);
        if( i < n+10 ) {
            printf("\t");
        }
    }

    return 0;
}
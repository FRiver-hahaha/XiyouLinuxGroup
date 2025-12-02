// 10

#include<stdio.h>

int main() {
    int n;
    int ch;
    do{
        scanf("%d %d",&n,&ch);
        if( ch == 1 ) {
            if( 17850 - n >= 0 ) {
                printf("%lf",( 17850 - n )*0.15);
            }else {
                printf("%lf",17850*0.15 + (n - 17850 )*0.28);
            }
        }
        if( ch == 2 ) {
            if( 23900 - n >= 0 ) {
                printf("%lf",( 23900 - n )*0.15);
            }else {
                printf("%lf",23900*0.15 + (n - 23900 )*0.28);
            }
        }
        if( ch == 3 ) {
            if( 29750 - n >= 0 ) {
                printf("%lf",( 29750 - n )*0.15);
            }else {
                printf("%lf",29750*0.15 + (n - 29750 )*0.28);
            }
        }
        if( ch == 4 ) {
            if( 14875 - n >= 0 ) {
                printf("%lf",( 14875 - n )*0.15);
            }else {
                printf("%lf",14875*0.15 + (n - 14875 )*0.28);
            }
        }
    }while(n != -1);

    return 0;
}
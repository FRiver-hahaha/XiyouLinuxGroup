// 18

#include<stdio.h>

int main() {
    int n = 5;
    int i=1;
    do{
        n-=i;
        n*=2;
        printf("%d",n);
        if( n <= 150 ) {
            printf("\t");
        }
        i++;
    }while( n <= 150 );

    return 0;
}
#include<stdio.h>

int isprime(int x);

int main() {
    int n;
    do {
        scanf("%d",&n);
        int i;
        for( i = 2; i <= n; i++ ) {
            if(isprime(i)) {
                printf("%d\t",i);
            }
            
        }
    }while( n < 0);

    return 0;
}

int isprime(int x) {
    if( x < 2 )return 0;
    if( x == 2 )return 1;
    if(x % 2 == 0 )return 0;
    for( int i = 3; i * i <= x; i+=2) {
        if( x % i == 0 ) {
            return 0;
        }
    }
    return 1;
}
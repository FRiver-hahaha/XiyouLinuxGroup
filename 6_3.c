// 13

#include<stdio.h>

int pow1(int a,int b);

int main() {
    int ar[8];
    int i;
    for( i = 0; i <= 7; i++) {
        ar[i] = pow1(2,i+1);
    }
    i = 0;
    do{
        printf("%d\t",ar[i]);
        i++;
    }while( i <= 7);

    return 0;
}

int pow1(int a,int b) {
    int i;
    int tmp1 = 1;
    for( i = 1; i <= b; i++) {
        tmp1*=a;
    }
    return tmp1;
}
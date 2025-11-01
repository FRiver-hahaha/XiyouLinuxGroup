#include<stdio.h>

int main() {
    int n;
    scanf("%d",&n);
    int w = n / 7;
    int d = n % 7;
    printf("%d days are %d weeks, %d days.",n,w,d);

    return 0;
}
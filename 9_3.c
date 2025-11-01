#include<stdio.h>

int Fibonacci(int x);

int main() {
    int n;
    scanf("%d",&n);

    printf("%d",Fibonacci(n));

    return 0;
}

int Fibonacci(int x) {
    if(x <= 2) return 1;

    return Fibonacci(x-1) + Fibonacci(x-2);
}
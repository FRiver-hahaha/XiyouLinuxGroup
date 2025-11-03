#include<stdio.h>

double plus(double x);

int main() {
    double n;
    scanf("%lf",&n);
    printf("%lf",plus(n));

    return 0;
}

double plus(double x) {
    return x*x*x;
}
#include<stdio.h>

double min(double x,double y);

int main() {
    double a = 1.5,b = 2.5;
    printf("%.1lf",min(a,b));
    return 0;
}

double min(double x,double y) {
    return (x < y) ? x : y;
}
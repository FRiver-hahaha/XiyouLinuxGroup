#include<stdio.h>
#define SIZE 10

void reverse(double* p);

int main() {
    double ar[SIZE] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.1};
    int i;

    for(i = 0; i < SIZE; i++) {
        printf("%lf\t",ar[i]);
    }
    
    reverse(ar);
    
    for(i = 0; i < SIZE; i++) {
        printf("%lf\t",ar[i]);
    }

    return 0;
}

void reverse(double* p) {
    int i;
    for(i = 0;i < SIZE / 2; i++) {
        double tmp = p[i];
        p[i] = p[SIZE - i -1];
        p[SIZE - i -1] = tmp;
    }
}
#include<stdio.h>
#include<stdlib.h>

double* transform(double* ar1, double* ar2, int len, double (*fp)(double));
double dob(double arr);
double rev(double ar);
double sqrt(double ar);

int main() {
    double ar1[] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,888.1,8.5,4.9};
    int len = sizeof(ar1) / sizeof(ar1[0]);
    double* ar2 = (double*)malloc(sizeof(double)*len);
    ar2 = transform(ar1, ar2, len, dob);
    ar2 = transform(ar1, ar2, len, rev);
    ar2 = transform(ar1, ar2, len, sqrt);
    free(ar2);
    return 0;
}

double dob(double arr) {
    return arr*2.0;
}

double rev(double ar) {
    return -ar;
}

double* transform(double* ar1, double* ar2, int len, double (*fp)(double)) {
    int i;
    
    for(i = 0; i < len ;i++) {
        ar2[i] = fp(ar1[i]);
    }
    return ar2;
}

double sqrt(double ar) {
    return ar*ar;
}
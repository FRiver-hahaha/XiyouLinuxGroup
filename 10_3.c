#include<stdio.h>
#include<stdlib.h>

void sum(int* x,int* y,int* z,int size);

int main() {
    int ar1[] = {1,2,3,4,7,5,8,6,9,5};
    int ar2[] = {2,4,5,8,5,8,4,6,9,3};
    int size = sizeof(ar1) / sizeof(ar1[0]);
    int* ar3 = (int*)malloc(sizeof(int)*size);
    sum(ar1,ar2,ar3,size);
    int i;

    for(i = 0; i < size; i++) {
        printf("%d\t",ar3[i]);
    }
    free(ar3);

    return 0;
}

void sum(int* x,int* y,int* z,int size) {
    int i;
    for(i = 0; i < size ;i++) {
        z[i] = x[i] + y[i];
    }
}
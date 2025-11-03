#include<stdio.h>
#define SIZE 10

int max(int* a);

int main() {
    int ar[SIZE] = {5,7,1,2,8,6,9,5,4,2};
    printf("%d",max(ar));

    return 0;
}

int max(int* a) {
    int max = a[0];
    int i;

    for(i = 0; i < SIZE; i++) {
        if(a[i] > max) {
            max = a[i];
        }
    }
    return max;
}
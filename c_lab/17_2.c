#include<stdio.h>
#include<stdlib.h>

int search(int* ar,int len,int value);

int main() {
    int ar[] = {1,2,2,3,4,5,6,7,8};
    int len = sizeof(ar) / sizeof(ar[0]);
    printf("%d\t",search(ar,len,2));
    printf("%d\n",search(ar,len,9));

    return EXIT_SUCCESS;
}

int search(int* ar,int len,int value) {
    int left = 0;
    int right = len;
    int middle = 0;

    while(left <= right) {
        middle = left + (right - left) / 2;
        if(value == ar[middle]) {
            return 1;
        }
        if(value > ar[middle]) {
            left = middle + 1;
        }
        if(value < ar[middle]) {
            right = middle - 1;
        }
    }
    return 0;
}
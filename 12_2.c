#include<stdio.h>
#include<stdlib.h>
#include<time.h>


int main() {
    srand((unsigned int)time(NULL));
    int i,j,key;
    int arr[100] = {};
    for(i = 0; i < 100; i++) {
        arr[i] = rand() % 10 + 1;
    }
    
    for(i = 1; i < 100; i++) {
        key = arr[i];
        j = i - 1;
        while(j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
    
    for(i = 0; i < 100; i++) {
        printf("%d",arr[i]);
        if(!(i == 99)) {
            printf(" ");
        }
    }

    return 0;
}
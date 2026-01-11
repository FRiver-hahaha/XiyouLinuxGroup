#include <stdio.h>
#include <stdlib.h>
#define SIZE 15

void printResult(int** ar);
void print(int* ar);
void sort1(int** ar);
void sort2(int** ar);
void sort3(int** ar, int low, int high);
void sort4(int** ar);

int main() {
    int fact[SIZE] = {1,5,2,7,5,9,8,8,5,1,9,6,10,8,58};
    int** arr = (int**)malloc(sizeof(int*) * SIZE);

    for(int i = 0; i < SIZE; ++i) {
        arr[i] = fact + i;
    }

    print(fact);
    sort1(arr);
    printResult(arr);

    for(int i = 0; i < SIZE; ++i) {
        arr[i] = fact + i;
    }

    print(fact);
    sort2(arr);
    printResult(arr);

    for(int i = 0; i < SIZE; ++i) {
        arr[i] = fact + i;
    }

    print(fact);
    sort3(arr, 0, SIZE - 1);
    printResult(arr);

    for(int i = 0; i < SIZE; ++i) {
        arr[i] = fact + i;
    }

    print(fact);
    sort4(arr);
    printResult(arr);

    free(arr);

    return 0;
}

void sort4(int** ar) {
    int i,j;
    for(i = 1; i < SIZE; ++i) {
        if(*ar[i] < *ar[i - 1]) {
            int* tmp = ar[i];
            for(j = i - 1; j >= 0 && *ar[j] > *tmp; j--) {
                ar[j + 1] = ar[j];
            }
            ar[j + 1] = tmp;
        }
    }
}

void sort3(int** ar, int low, int high) {
    int i = low;
    int j = high;
    int* key = ar[low];
    if(i > j) {
        return;
    }
    while(i < j) {
        while(i < j && *ar[j] >= *key) {
            j--;
        }
        while(i < j && *ar[i] <= *key) {
            i++;
        }
        if(i < j) {
            int* ptr = ar[i];
            ar[i] = ar[j];
            ar[j] = ptr;
        }
    }
    int* ptr = ar[i];
    ar[i] = ar[low];
    ar[low] = ptr;
    sort3(ar, low, i - 1);
    sort3(ar, i + 1, high);
}


void sort2(int** ar) {
    int i,j;
    int min = 0;
    for(i = 0; i < SIZE; ++i) {\
        min = i;
        for(j = i; j < SIZE; ++j) {
            if(*ar[j] < *ar[min]) {
                min = j;
            }
        }
        int* ptr = ar[i];
        ar[i] = ar[min];
        ar[min] = ptr;
    }
}

void sort1(int** ar) {
    int i,j;
    for(i = 0; i < SIZE - 1; ++i) {
        for(j = 0; j < SIZE - 1 - i; ++j) {
            if(*ar[j] > *ar[j + 1]) {
                int* ptr = ar[j];
                ar[j] = ar[j + 1];
                ar[j + 1] = ptr;
            }
        }
    }
}

void printResult(int** ar) {
    int i;
    for(i = 0; i < SIZE; i++) {
        printf("%d ",*ar[i]);
    }
    printf("\n");
}

void print(int* ar) {
    for(int i = 0; i < SIZE; ++i) {
        printf("%d ",ar[i]);
    }
    printf("\n");
}

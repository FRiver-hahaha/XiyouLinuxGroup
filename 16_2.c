// 5

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void filter(int*,int,int);

int main() {
    srand((unsigned)time(NULL));
    int ar[10];
    int i;
    for(i = 0; i < 10; i++) {
        ar[i] = rand() % 20 + 1;
        printf("%d",ar[i]);
        if(!(i == 9)) {
            printf("\t");
        }
    }
    printf("\n");
    int number = rand() % 10 + 1;
    printf("%d\n",number);
    filter(ar,10,number);


    return 0;
}

void filter(int* p,int size,int number) {
    int i;
    for(i = 0; i < number; i++) {
        printf("%d",p[i]);
        if(!(i == (number - 1))) {
            printf("\t");
        }
    }
}
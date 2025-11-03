#include<stdio.h>

int main(int argc, char *argv[]) {
    int i;
    for(i = argc; i >= 1; i--) {
        if(!argv[i]) {
            continue;
        }
        printf("%s",argv[i]);
        if(i != 1) {
            printf(" ");
        }
    }

    return 0;
}
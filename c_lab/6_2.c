#include<stdio.h>

int main() {
    char ch = '$';
    int i,j;
    for( i = 0; i < 5; i++) {
        for( j = 0; j <= i; j++) {
            printf("%c",ch);
        }
        printf("\n");
    }
    return 0;
}
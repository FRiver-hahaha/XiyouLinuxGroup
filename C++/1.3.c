#include <stdio.h>

int main() {
    int i;
    int sum = 0;
    char ch;
    while(scanf("%d",&i) == 1) {
        sum += i;
        while((ch = getchar()) == ' ');

        if(ch == '\n') break;

        ungetc(ch, stdin);
    }

    printf("sum = %d",sum);

    return 0;
}
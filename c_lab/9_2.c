#include<stdio.h>

int ret(char ch);

int main() {
    char ch;
    while(scanf("%c",&ch) != EOF) {
        if(ch == 's') {
            break;
        }
        while(getchar() != '\n') {
            continue;
        }
        printf("%d\n",ret(ch));
    }

    return 0;
}

int ret(char ch) {
    if(ch >= 65 && ch <= 65+26) {
        return ch-'A';
    }
    if(ch >= 97 && ch <= 97+26) {
        return ch-'a';
    }
    return -1;
}
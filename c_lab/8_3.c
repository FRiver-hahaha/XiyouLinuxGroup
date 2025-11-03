#include<stdio.h>

int main() {
    int guess = 50;
    int n =100;
    char ch;

    printf("Pick an integer from 1 to 100. I will try to guess");
    printf("it.\nRespond whie a y if my guessi is right and with");
    printf("\nan b if is bigger and an s if is smaller.\n");
    printf("Uf...is your number %d\n",guess);
    while((ch = getchar()) != 'y') {
        if(ch == 'b') {
            guess = (guess-n/2-1 + guess)/2;
        }
        if(ch == 's') {
            guess = (guess+n/2+1 + guess)/2;
        }
        printf("Well, then, is it %d?\n", guess);
        n/=2;
        while(getchar() != '\n') {
            continue;
        }
    }
    printf("I knew I could do it!\n");

    return 0;
}
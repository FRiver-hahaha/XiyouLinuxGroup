#include<stdio.h>

int main() {
    
    int sum = 0;
    int i = 0;
    char ch;

    while(scanf("%d", &i) == 1) {
        sum += i;
        while((ch = getchar()) == ' ');
        if(ch == '\n') {
            break;
        }
    }

    printf("%d\n",sum);
    return 0;
}

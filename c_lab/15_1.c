// 2

#include<stdio.h>
#include<limits.h>
#include<string.h>

int init(char*);
void pBitstr(int,char*);
int pow(int x,int y);

int main(int argc,char* argv[]) {
    int sum1,sum2;
    char bitStr[CHAR_BIT* sizeof(int) + 1];
    sum1 = init(argv[1]);
    sum2 = init(argv[2]);
    pBitstr(~(sum1 + sum2),bitStr);
    pBitstr(sum1 & sum2,bitStr);
    pBitstr(sum1 | sum2,bitStr);
    pBitstr(sum1 ^ sum2,bitStr);

    return 0;
}

int init(char* str) {
    int i = 0;
    int sum = 0;
    int len = strlen(str);
    while(str[i] != '\0') {
        sum += pow(2,len - i - 1) * (str[i]  - '0');
        i++;
    }
    return sum;
}

void pBitstr(int n,char* str) {
    int i;
    const static int size = sizeof(int) * CHAR_BIT;
    for(i = size - 1; i >= 0; i--,n >>= 1) {
        str[i] = (1 & n) + '0';
    }
    str[size] = '\0';
    i = 0;
    while(str[i]) {
        putchar(str[i]);
        if(str[i] && ++i % 4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

int pow(int x,int y) {
    int tmp = x;
    int i;
    for(i = 0; i < y; i++) {
        x *= tmp;
    }
    x /= tmp;
    return x;
}
// 5

#include<stdio.h>
#include<limits.h>

void bitS(char*,int);
void bitChange(char*,int);
void Pbit(char*);

static int size = CHAR_BIT * sizeof(int);

int main() {
    unsigned int n;
    int m;
    scanf("%d %d",&n,&m);
    char bitStr[CHAR_BIT * sizeof(int) + 1];
    bitS(bitStr,n);
    Pbit(bitStr);

    bitChange(bitStr,m);

    Pbit(bitStr);

    return 0;
}

void bitS(char* str,int n) {
    int i;
    for(i = size - 1; i >= 0; i--, n >>= 1) {
        str[i] = (01 & n) + '0';
    }
    str[size] = '\0';
}

void bitChange(char* str,int m) {
    int i,j;
    for(i = 0; i < m; i++) {
        for(j = 0; j < size - 1; j++) {
            int tmp = str[j];
            str[j] = str[j+1];
            str[j+1] = tmp;
        }
    }
}

void Pbit(char* str) {
    int i = 0;
    while(str[i]) {
        putchar(str[i]);
        if(str[i] && ++i % 4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}
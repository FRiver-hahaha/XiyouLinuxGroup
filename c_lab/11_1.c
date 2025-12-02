// 9

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int main() {
    char* s = "abcdefg";
    char* s1;
    int len = strlen(s);
    s1 = (char*)malloc(sizeof(char)*len);
    int i;
    for(i = 0; i < len; i++) {
        s1[i] = s[len-1-i];
    }
    printf("%s",s1);
    free(s1);
    s1 = NULL;

    return 0;
}

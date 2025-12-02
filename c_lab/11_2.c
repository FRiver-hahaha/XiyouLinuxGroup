// 16

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void Toupper(char* s);
void Tolower(char* s);

int main(int argc,char *argv[]) {
    FILE* fp;
    fp = fopen("test.txt","r");
    char s[100];
    while(fgets(s,100,fp) != NULL) {
        if(argc > 1) {
            int tmp;
            if(strcmp(argv[1],"-p") == 0) {
                tmp = 0;
            }
            if(strcmp(argv[1],"-u") == 0) {
                tmp = 1;
            }
            if(strcmp(argv[1],"-l") == 0) {
                tmp = 2;
            }
            switch(tmp) {
                case 1:Toupper(s);break;
                case 2:Tolower(s);break;
                default:0;break;
            }
        }
    }

    printf("%s",s);
    fclose(fp);
    
    return EXIT_SUCCESS;
}

void Toupper(char* s) {
    int len = strlen(s);
    int i;
    for(i = 0; i < len; i++) {
        if(s[i] >= 97 && s[i] <= 97+26) {
            s[i] -= 32;
        }
    }
}


void Tolower(char* s) {
    int len = strlen(s);
    int i;
    for(i = 0; i < len; i++) {
        if(s[i] >= 65 && s[i] <= 65+26) {
            s[i] += 32;
        }
    }
}
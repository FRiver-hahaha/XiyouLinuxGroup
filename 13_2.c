#include<stdio.h>
 
int main(int argc, char *argv[]) {
    FILE* fp1, * fp2;
    char name[100];
    fp1 = fopen(argv[1],"r");
    if(argc < 2) {
        scanf("%s",name);
        fp2 = fopen(name,"r");
    }
    fclose(fp1);
    fclose(fp2);

    return 0;
}
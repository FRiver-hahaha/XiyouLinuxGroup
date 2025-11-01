#include<stdio.h>
#include<ctype.h>
#include<string.h>

int main() {
    FILE * fp1, * fp2;
    char name[100];
    char Tmpname[20] = "textTmp.txt";
    scanf("%s",name);
    if((fp1 = fopen(name,"r")) == NULL) {
        printf("NOT FOUND");
        return 0;
    }

    if((fp2 = fopen(Tmpname,"w")) == NULL) {
        printf("NOT FOUND");
        return 0;
    }
    char c;
    while((c = fgetc(fp1)) != EOF) {
        // fprintf(fp,"%c",toupper(c));
        fputc(toupper(c),fp2);
    }   

    fclose(fp1);
    fclose(fp2);
    remove(name);
    rename(Tmpname,name);

    return 0;
}
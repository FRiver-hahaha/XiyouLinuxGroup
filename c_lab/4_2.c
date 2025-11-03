#include<stdio.h>
#include<string.h>

int main()
{
    char name1[100],name2[100];
    scanf("%s %s",name1,name2);
    printf("%s %s\n",name1,name2);
    int len1 = strlen(name1);
    int len2 = strlen(name2);
    int i;
    for (i = 0;i < len1-1;i++) {
        printf(" ");
    }
    
    printf("%d ",len1);

    for (i = 0;i < len2-1;i++) {
        printf(" ");
    }

    printf("%d\n",len2);

    printf("%s %s\n",name1,name2);
    printf("%d",len1);

    for (i = 0;i < len1-1;i++) {
        printf(" ");
    }

    printf(" %d",len2);

    return 0;
}
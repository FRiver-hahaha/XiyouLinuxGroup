#include <stdio.h>

int main() {
    int i;
    int sum = 0;
    char ch;
    while(scanf("%d",&i) == 1) {
        sum += i;
        while((ch = getchar()) == ' ');// 清理空格，但缺点是会将数据损坏

        if(ch == '\n') break;

        ungetc(ch, stdin);// 将ch获取到的数据回退给标准输入内
    }

    printf("sum = %d",sum);

    return 0;
}
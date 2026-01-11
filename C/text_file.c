#include <stdio.h>

int f(int x) {
    if(x == 0 || x == 1) {
        
        return 1;
    }
    printf("当前x = %d\n",x);
    return f(x - 1) + f(x - 2);
}

int main() {
    int n;
    printf("请输入待计算数字：");
    scanf("%d",&n);
    printf("sum = %d\n",f(n));

    return 0;
}
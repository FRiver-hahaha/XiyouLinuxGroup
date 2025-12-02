// 3

#include<stdio.h>

int main() {
    int number;
    int ans_o = 0;
    int ans_j = 0;
    int cnt_o = 0;
    int cnt_j = 0;
    while( scanf("%d",&number) ==1 && number != 0 ) {
        if( number % 2 == 0 ) {
            cnt_o++;
            ans_o += number;
            printf("cnt_o==%d ans_o==%d\n",cnt_o,ans_o/cnt_o);
        }else {
            cnt_j++;
            ans_j += number;
            printf("cnt_j==%d ans_j==%d\n",cnt_j,ans_j/cnt_j);
        }
    }

    return 0;
}
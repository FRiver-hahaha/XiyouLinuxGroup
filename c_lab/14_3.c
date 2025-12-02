// 2

#include<stdio.h>

typedef struct time {
    int day;
    int month;
    int year;
}Time;


int main() {
    Time time;
    int sum = 0;
    printf("Input your current day,month,year:");
    scanf("%d %d %d",&time.day,&time.month,&time.year);
    if(!(time.month == 1)) {
        sum += (time.month - 1) * 30;
        sum += time.day;
    }
    else {
        sum += time.day;
    }


    return 0;
}

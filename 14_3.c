#include<stdio.h>

typedef struct time {
    int day;
    int month;
    int year;
}Time;

int isleap(int x);

int main() {
    Time time;
    int sum = 0;
    printf("Input your current day,month,year:");
    scanf("%d %d %d",&time.day,&time.month,&time.year);
    if(isleap(time.year)) {
        int i = 1;
        while(i <= time.month) {
            if(time.month == 2) {
                sum += time.day;
            }
            else if(time.month == 1 || time.month == 3 || time.month == 5 || time.month == 7 || time.month == 8 || time.month == 10 || time.month == 12) {
                sum += time.day;
            }else {
                sum += 30;
            }
        }
        
    }

    return 0;
}

int isleap(int x) {
    if(x%4 == 0 && x%400 != 0 || x%400 == 0) {
        return 1;
    }
    return 0;
}
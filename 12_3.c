// 1

#include<stdio.h>

void critic(int* a);

int main() {
    static int units = 0;
    printf("How many pounds to a firkin of butter?\n");
    scanf("%d",&units);
    while(units != 56) {
        critic(&units);
    }
    printf("you must have looked it up!");
    return 0;
}

void critic(int* a) {
    printf("No luck, my friend.Try again.\n");
    scanf("%d",a);
}
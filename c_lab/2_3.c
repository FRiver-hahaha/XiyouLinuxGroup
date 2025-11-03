#include<stdio.h>

void one_three();
void two();

int main () {
    printf("starting now:\n");
    one_three();
    two();
    printf("three\n");
    printf("done!");
    return 0;
}

void one_three () {
    printf("one\n");
}

void two () {
    printf("two\n");
}
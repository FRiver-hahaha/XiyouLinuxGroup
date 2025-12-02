// 1

#include<stdio.h>
#include<stdlib.h>
#include"16_1H.h"

void add(List* pList, int number);
void clear(List* pList);

int main() {
    List list;
    list.head = NULL;
    list.tail = NULL;
    int i = 0;
    int number;
    Node* p;

    do {
        scanf("%d",&number);
        i++;
        add(&list,number);
    }while(i < SIZE);
    while(getchar() != '\n');
    puts("helloworld");
    for(p = list.head; p; p = p->next) {
        printf("%d\t",p->value);
    }

    clear(&list);

    return EXIT_SUCCESS;
}

void clear(List* pList) {
    Node* p,* q;
    p = NULL;
    q = NULL;
    for(p = pList->head; p; p = q) {
        q = p->next;
        free(p);
        p = NULL;
    }
    pList->head = NULL;
    pList->tail = NULL;
}

void add(List* pList, int number) {
    Node* p = (Node*)malloc(sizeof(Node));
    p->value = number;
    p->next = NULL;
    if(!pList->head) {
        pList->head = p;
        pList->tail = p;
    }
    else {
        pList->tail->next = p;
        pList->tail = p;
    }
}
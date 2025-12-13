#include <stdio.h>
#include <stdlib.h>

typedef struct _node {
    int value;
    struct _node* next;
}Node;

typedef struct {
    Node* head;
}List;

void add(List* pList, int number);//尾插
void insert(List* pList, int number);//递增插入
void show(const List list);//展示
void del(List* pList, int number);//删除
void clear(List* pList);//清除

int main() {
    List list;
    list.head = (Node*)malloc(sizeof(Node));
    list.head->next = NULL;
    int number;
    do {
        scanf("%d",&number);
        if(number != -1) {
            add(&list, number);
        }
    } while (number != -1);
    
    show(list);

    printf("search a number:");
    scanf("%d",&number);
    insert(&list, number);\
    show(list);

    printf("del numbers:");
    scanf("%d",&number);
    del(&list, number);
    show(list);

    clear(&list);
    return 0;
}

void del(List* pList, int number) {
    Node* p = pList->head;
    Node* del = NULL;
    while(p->next) {
        if(p->next->value != number) {
            p = p->next;
        }
        else {
            del = p->next->next;
            free(p->next);
            p->next = del;
        }
    }
}

void clear(List* pList) {
    Node* q = NULL;
    for(Node* p = pList->head; p; p = q) {
        q = p->next;
        free(p);
        p = NULL;
    }
}

void show(const List list) {
    Node* p = list.head;
    for(p = list.head->next; p; p = p->next) {
        printf("%d ",p->value);
    }
}

void add(List* pList, int number) {
    Node* p = (Node*)malloc(sizeof(Node));
    p->value = number;
    p->next = NULL;

    if(!pList->head->next) {
        pList->head->next = p;
    }else {
        Node* last = pList->head;
        while(last->next) {
            last = last->next;
        }
        last->next = p;
    }
}

void insert(List* pList, int number) {

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = NULL;
    newNode->value = number;

    if(pList->head->next == NULL) {
        pList->head->next = newNode;
        return;
    }

    Node* p = pList->head;
    while(p->next && p->next->value <= number) {
        p = p->next;
    }    
    newNode->next = p->next;
    p->next = newNode;
}
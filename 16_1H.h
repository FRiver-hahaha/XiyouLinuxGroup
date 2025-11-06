// 1

#ifndef _16_1H_H_
#define _16_1H_H_
#define SIZE 10


typedef struct _node {
    int value;
    struct _node* next;
}Node;

typedef struct {
    Node* head;
    Node* tail;
}List;

#endif
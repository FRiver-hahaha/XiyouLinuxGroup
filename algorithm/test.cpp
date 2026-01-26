#include <iostream>
using namespace std;

typedef struct _node {
    int value;
    struct _node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
}List;

void add(List* pList, int n);
void yoself(List* pList, int m);
void clear(List* pList);

int main() {
    List list;
    int n;
    int m;
    cin >> n >> m;
    add(&list, n);
    yoself(&list, m);
    clear(&list);
    return 0;
}

void clear(List* pList) {
    Node* p = pList->head;
    Node* q = NULL;
    for(p = pList->head; p; p = q) {
        q = p->next;
        free(p);
    }
}

void yoself(List* pList, int m) {
    Node* p = pList->head;
    int count = 1;
    while(p->next) {
        if(count != m) {
            p = p->next;
            ++count;
        }else {
            cout << p->next->value << ' ';
            Node* tmp = p->next->next;
            free(p->next);
            p->next = tmp;
            count = 1;
        }
    }
}

void add(List* pList, int n) {
    pList->head = (Node*)malloc(sizeof(Node));
    pList->tail = (Node*)malloc(sizeof(Node));
    int i;
    for(i = 1; i <= n; ++i) {
        Node* p = (Node*)malloc(sizeof(Node));
        p->value = i;
        p->next = NULL;
        if(!pList->head) {
            pList->head->next = p;
            pList->tail = p;
        }else {
            pList->tail->next = p;
            pList->tail = p;
        }
    }
    pList->tail->next = pList->head->next;
}
//linked list
#include<stdio.h>
#include<stdlib.h>

typedef struct _node
{
    int value;
    struct _node* next;
}Node;

typedef struct {
    Node* head;
}List;

List init(void);//创建
void add(List* pList,int number);//尾插
void search(List* pList,int number);//删查
void reverse(List* pList);//翻转
void print(const List list);//打印
void delete(List* pList);//清空

int main() {
    List list = init();
    int number;
    do
    {
        scanf("%d",&number);
        if(number != -1) {
            add(&list, number);
        }
    } while (number != -1);
    
    print(list);

    scanf("%d",&number);
    search(&list,number);
    print(list);
    printf("\n");
    reverse(&list);
    print(list);
    delete(&list);

    return 0;
}

List init(void) {
    List list;
    list.head = NULL;
    return list;
}

void add(List* pList,int number) {
    Node* p = (Node*)malloc(sizeof(Node));
    p->value = number;
    p->next = NULL;
    if(!pList->head) {
        pList->head = p;
    }else {
        Node* last = pList->head;
        while(last->next) {
            last = last->next;
        }
        last->next = p;
    }
}

void search(List* pList,int number) {
    Node* p = pList->head;
    Node* q = pList->head->next;
    while(pList->head->value == number) {
        Node* tmp = pList->head;
        pList->head = pList->head->next;
        free(tmp);
        tmp = NULL;
    }

    while(q) {
        if(q->value == number) {
            p->next = q->next;
            free(q);
            q = p->next;
        }else {
            p = q;
            q = q->next;
        }
    }
}

void delete(List* pList) {
    Node* p = pList->head;
    Node* q = NULL;
    for(p = pList->head; p; p = q) {
        q = p->next;
        free(p);
        p = NULL;
    }
}

void print(const List list) {
    Node* p;
    for(p = list.head; p; p = p->next) {
        printf("%d\t",p->value);
    }
}

void reverse(List* pList) {
    Node* prev = NULL;
    Node* curr = pList->head;
    Node* next = NULL;
    if(pList->head == NULL || pList->head->next == NULL) {
        return;
    }
    else {
        while(curr) {
            next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }

        pList->head = prev;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 20
#define NIL -1

struct List
{
    int* data;
    
    int* next;

    int head;
    int tail;

    int free_head;
};

int PushAfter(List* list, int prev, int value);

void InitList(List* list);

void ListPrint(const List* list);

int DeleteNext(List* list, int prev);

int main()
{
    List list = {};
    InitList(&list);

    PushAfter(&list, 0, 10);
    PushAfter(&list, 1, 20);
    PushAfter(&list, 0,  5);

    DeleteNext(&list, 3);

    ListPrint(&list);  
}

int PushAfter(List* list, int prev, int value)
{
    int idx = NIL;
    for (int i = 1; i < CAPACITY; i++)
    {
        if (list->next[i] == NIL)
        {
            idx = i;
            break;
        }
    }

    if (idx == NIL) return 1;

    list->data[idx] = value;

    list->next[idx]  = list->next[prev];
    list->next[prev] = idx;

    if (list->tail == prev)
        list->tail = idx;

    return 0;
}

void InitList(List* list)
{
    list->data = (int*) calloc(CAPACITY, sizeof(int));
    list->next = (int*) malloc(CAPACITY * sizeof(int));

    for (int i = 0; i < CAPACITY; i++)
        list->next[i] = NIL;


    list->next[0]  = 0;
    list->head     = 0;
    list->tail     = 0;
    
}

void ListPrint(const List* list)
{
    printf("Head");
    for (int i = list->next[0]; i != 0; i = list->next[i])
        printf(" -> %d", list->data[i]);
}

int DeleteNext(List* list, int prev)
{
    if (list->next[prev] == NIL) return -printf("Delate error\n");

    
    
    list->next[prev] = list->next[list->next[prev]];
    return 0;
}


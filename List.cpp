#include "List.h"


static int Insert(List* list, int prev, int value)
{
    if (ListVerifyer(list) != OK) return 1;

    int idx = list->free_head;
    if (idx == NIL) return 1;
    list->free_head = list->next[idx];

    list->data[idx] = value;

    int nxt = list->next[prev];
    list->next[idx]  = nxt;
    list->prev[idx]  = prev;
    list->next[prev] = idx;

    list->prev[nxt] = idx;

    return 0;
}

int InsertAfter(List* list, int prev, int value)
{
    if (ListVerifyer(list) != OK) return 1;

    int rc = Insert(list, prev, value);
    if (rc == 0 && ListVerifyer(list) != OK) return 1;
    return rc;
}

int InsertBefore(List* list, int idx, int value)
{
    if (ListVerifyer(list) != OK) return 1;
    
    if (idx == 0) return 1;
    if (idx < 1 || idx >= CAPACITY || list->prev[idx] == NIL) return 1;

    int rc = Insert(list, list->prev[idx], value);
    if (rc == 0 && ListVerifyer(list) != OK) return 1;
    return rc;
}

int InitList(List* list)
{
    list->data = (int*) calloc(CAPACITY, sizeof(int));
    list->next = (int*) malloc(CAPACITY * sizeof(int));
    list->prev = (int*) malloc(CAPACITY * sizeof(int));

    if (!list->data || !list->next || !list->prev) {fprintf(stderr, "MEMORY_ERROR\n"); return 1;}

    for (int i = 1; i < CAPACITY - 1; i++)
        list->next[i] = i + 1;
    list->next[CAPACITY - 1] = NIL;

    for (int i = 0; i < CAPACITY; i++)
        list->prev[i] = NIL;

    list->next[0]   = 0;
    list->prev[0]   = 0;
    list->free_head = 1;

    if (ListVerifyer(list) != OK) return ListVerifyer(list);
    
    return OK;
}

void ListPrint(const List* list)
{
    if (ListVerifyer(list) != OK) return;

    printf("Head");
    for (int i = list->next[0]; i != 0; i = list->next[i])
        printf(" -> %d", list->data[i]);
    printf("\n");
}

int DeleteNext(List* list, int prev)
{
    if (ListVerifyer(list) != OK) return -1;

    int victim = list->next[prev];
    if (victim == 0) return 1;

    list->next[prev] = list->next[victim];
    list->next[victim] = list->free_head;
    list->free_head = victim;


    return 0;
}

int DeleteElem(List* list, int idx)
{
    if (ListVerifyer(list) != OK) return -1;

    if (idx == 0 || list->prev[idx] == NIL) return 1;

    list->next[list->prev[idx]] = list->next[idx];
    list->prev[list->next[idx]] = list->prev[idx];
    

    list->next[idx] = list->free_head;
    list->prev[idx] = NIL;
    list->free_head = idx;

    return 0;
}

int ListDtor(List* list)
{
    if (!list) return 1;

    free(list->data);
    free(list->next);
    free(list->prev);
    list->free_head = 0;
    
    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;
    list = NULL;

    return 0;
}


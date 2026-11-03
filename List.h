#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 20
#define NIL -1

struct List
{
    int* data;
    
    int* next;
    int* prev;

    int free_head;
};

enum Errors
{
    OK = 0,
    ZERO_NEXT = 1,
    ZERO_PREV = 2,
    FORWARD   = 3,
    BACKWARD  = 4,
    WRONG_PREV_NEXT = 5,
    EMPTY_ERROR  = 6,
    TAIL = 7,
    HEAD = 8,
    FREE_HEAD = 9,
    FREE = 10,
};

int PushAfter(List* list, int prev, int value);

int InitList(List* list);

void ListPrint(const List* list);

int DeleteNext(List* list, int prev);

int DeleteElem(List* list, int idx);

Errors ListVerifyer(const List* list);

int PushAfter(List* list, int prev, int value)
{
    if (ListVerifyer(list) != OK) return -1;

    int idx = list->free_head;
    if (idx == NIL) return NIL;
    list->free_head = list->next[idx];

    list->data[idx] = value;

    int nxt = list->next[prev];
    list->next[idx] = nxt;
    list->prev[idx] = prev;
    list->next[prev] = idx;
    
    list->prev[nxt] = idx;

    if (ListVerifyer(list) != OK) return -1;

    return 0;
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

#endif
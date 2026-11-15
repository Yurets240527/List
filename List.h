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

int InitList(List* list);

void ListPrint(const List* list);

int DeleteNext(List* list, int prev);

int DeleteElem(List* list, int idx);

int InsertAfter(List* list, int prev, int value);

int InsertBefore(List* list, int idx, int value);

Errors ListVerifyer(const List* list);

int ListDtor(List* list);

#endif
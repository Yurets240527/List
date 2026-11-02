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

void InitList(List* list);

void ListPrint(const List* list);

int DeleteNext(List* list, int prev);

int DeleteElem(List* list, int idx);

Errors ListVerifyer(const List* list);

int main()
{
    List list = {};
    InitList(&list);

    PushAfter(&list, 0, 10);
    PushAfter(&list, 1, 20);
    PushAfter(&list, 0,  5);

    DeleteElem(&list, 1);

    ListPrint(&list);  
}

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

void InitList(List* list)
{
    list->data = (int*) calloc(CAPACITY, sizeof(int));
    list->next = (int*) malloc(CAPACITY * sizeof(int));
    list->prev = (int*) malloc(CAPACITY * sizeof(int));

    for (int i = 1; i < CAPACITY - 1; i++)
        list->next[i] = i + 1;
    list->next[CAPACITY - 1] = NIL;

    for (int i = 0; i < CAPACITY; i++)
        list->prev[i] = NIL;

    list->next[0]   = 0;
    list->prev[0]   = 0;
    list->free_head = 1;
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

Errors ListVerifyer(const List* list)
{
    int state[CAPACITY];
    for (int i = 0; i < CAPACITY; i++)
        state[i] = 0;

    if (list->next[0] != 0 &&
        (list->next[0] < 1 || list->next[0] >= CAPACITY)) {
        fprintf(stderr, "FAIL 1: next[0]=%d невалиден\n", list->next[0]);
        return ZERO_NEXT;
    }

    if (list->prev[0] != 0 &&
        (list->prev[0] < 1 || list->prev[0] >= CAPACITY)) {
        fprintf(stderr, "FAIL 2: prev[0]=%d невалиден\n", list->prev[0]);
        return ZERO_PREV;
    }

    int count_work = 0;
    int i = list->next[0];
    int prev_i = 0;
    while (i != 0) {
        if (i < 1 || i >= CAPACITY) {
            fprintf(stderr, "FAIL 3: next-обход: индекс %d вне диапазона\n", i);
            return FORWARD;
        }
        if (state[i] != 0) {
            fprintf(stderr, "FAIL 3: next-обход: цикл на узле %d\n", i);
            return FORWARD;
        }
        state[i] = 1;
        count_work++;

        if (list->prev[i] != prev_i) {
            fprintf(stderr, "FAIL 5a: prev[%d]=%d, ожидалось %d\n",
                    i, list->prev[i], prev_i);
            return WRONG_PREV_NEXT;
        }
        prev_i = i;
        i = list->next[i];
    }

    int count_back = 0;
    int j = list->prev[0];
    int next_j = 0;
    while (j != 0) {
        if (j < 1 || j >= CAPACITY) {
            fprintf(stderr, "FAIL 3b: prev-обход: индекс %d вне диапазона\n", j);
            return BACKWARD;
        }
        if (state[j] != 1) {
            fprintf(stderr, "FAIL 3b: prev-обход: узел %d не в рабочем списке\n", j);
            return BACKWARD;
        }
        count_back++;

        if (list->next[j] != next_j) {
            fprintf(stderr, "FAIL 5b: next[%d]=%d, ожидалось %d\n",
                    j, list->next[j], next_j);
            return WRONG_PREV_NEXT;
        }
        next_j = j;
        j = list->prev[j];
    }

    if (count_work != count_back) {
        fprintf(stderr, "FAIL 3c: вперёд %d узлов, назад %d\n",
                count_work, count_back);
        return FORWARD;
    }

    if (count_work == 0) {
        if (list->next[0] != 0 || list->prev[0] != 0) {
            fprintf(stderr, "FAIL 6: пустой список, но next[0]=%d prev[0]=%d\n",
                    list->next[0], list->prev[0]);
            return EMPTY_ERROR;
        }
    }

    if (list->prev[0] != 0 && list->next[list->prev[0]] != 0) {
        fprintf(stderr, "FAIL 7: prev[0]=%d, но next[%d]=%d (не хвост)\n",
                list->prev[0], list->prev[0], list->next[list->prev[0]]);
        return TAIL;
    }

    if (list->next[0] != 0 && list->prev[list->next[0]] != 0) {
        fprintf(stderr, "FAIL 8: next[0]=%d, но prev[%d]=%d (не голова)\n",
                list->next[0], list->next[0], list->prev[list->next[0]]);
        return HEAD;
    }

    if (list->free_head != NIL &&
        (list->free_head < 1 || list->free_head >= CAPACITY)) {
        fprintf(stderr, "FAIL 9: free_head=%d невалиден\n", list->free_head);
        return FREE_HEAD;
    }
    if (list->free_head != NIL && state[list->free_head] != 0) {
        fprintf(stderr, "FAIL 9: free_head=%d уже в рабочем списке\n",
                list->free_head);
        return FREE_HEAD;
    }

    int count_free = 0;
    int k = list->free_head;
    while (k != NIL)
    {
        if (k < 1 || k >= CAPACITY) {
            fprintf(stderr, "FAIL 10: free-обход: индекс %d вне диапазона\n", k);
            return FREE;
        }
        if (state[k] != 0) {
            fprintf(stderr, "FAIL 10: free-обход: узел %d уже посещён "
                    "(в рабочем списке?)\n", k);
            return FREE;
        }
        state[k] = 2;
        count_free++;

        if (list->prev[k] != NIL) {
            fprintf(stderr, "FAIL 10: prev[%d]=%d, ожидалось NIL\n",
                    k, list->prev[k]);
            return FREE;
        }
        k = list->next[k];
    }

    return OK;
}


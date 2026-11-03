#ifndef VERIFYER_H
#define VERIFYER_H

#include <stdio.h>
#include "List.h"

Errors ListVerifyer(const List* list)
{
    int state[CAPACITY] = {};

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

    int count_forward = 0;
    int i = list->next[0];
    int prev_i = 0;

    while (i != 0) {
        if (i < 1 || i >= CAPACITY) {
            fprintf(stderr, "FAIL 3: next-обход: индекс %d вне диапазона\n", i);
            return FORWARD;
        }
        
        if (count_forward >= CAPACITY)
        {
            fprintf(stderr, "FAIL 3: зацикленный список");
            return FORWARD;
        }

        if (state[i] != 0) {
            fprintf(stderr, "FAIL 3: next-обход: цикл на узле %d\n", i);
            return FORWARD;
        }

        state[i] = 1;
        count_forward++;

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

        if (count_back >= CAPACITY)
        {
            fprintf(stderr, "FAIL 4: зацикленный список");
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

    if (count_forward != count_back) {
        fprintf(stderr, "FAIL 3c: вперёд %d узлов, назад %d\n",
                count_forward, count_back);
        return FORWARD;
    }

    if (count_forward == 0) {
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


    return OK;
}


void DotDump(const List* list, const char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }

    fprintf(f, "digraph List {\n");
    fprintf(f, "  rankdir=LR;\n");
    fprintf(f, "  node [fontname=\"monospace\", fontsize=10, shape=Mrecord];\n");
    fprintf(f, "  edge [fontname=\"monospace\", fontsize=9];\n\n");

    for (int i = 0; i < CAPACITY; i++)
    {
        const char* fill;
        const char* style;
        if (i == 0) {
            fill = "#c10aee"; style = "filled";
        } else if (list->prev[i] == NIL) {
            fill = "#E8E8E8"; style = "filled,dashed";
        } else {
            fill = "#C0FFC0"; style = "filled";
        }
        fprintf(f, "  n%d [style=\"%s\", fillcolor=\"%s\", color=\"#008000\", "
                   "label=\"idx %d | val %d | next %d | prev %d\"];\n",
                i, style, fill, i, list->data[i], list->next[i], list->prev[i]);

        if (i < CAPACITY - 1)
            fprintf(f, "  n%d -> n%d [style=invis];\n", i, i + 1);
    }


    int first = (list->next[0] != 0) ? list->next[0] : 0;
    fprintf(f, "  HEAD [shape=Mrecord, style = filled, fillcolor = \"#eec705\", label=\"HEAD\"];\n");
    fprintf(f, "  { rank=same; HEAD; n%d; }\n", first);
    fprintf(f, "  HEAD -> n%d [style=invis];\n", first);
    fprintf(f, "  HEAD -> n%d [color=\"#0050C0\", constraint=false];\n", first);

    int last = (list->prev[0] != 0) ? list->prev[0] : 0;
    fprintf(f, "  TAIL [shape=Mrecord, style = filled, fillcolor = \"#eec705\", label=\"TAIL\"];\n");
    fprintf(f, "  { rank=same; TAIL; n%d; }\n", last);
    fprintf(f, "  TAIL -> n%d [style=invis];\n", last);
    fprintf(f, "  n%d -> TAIL [color=\"#0050C0\", constraint=false];\n", last);

    if (list->next[0] != 0)
        fprintf(f, "  n0 -> n%d [color=\"#0050C0\", penwidth=2, constraint=false];\n", list->next[0]);

    fprintf(f, "  TAIL -> n0 [color=\"#0050C0\", constraint=false, style=dashed];\n");
                

    for (int i = list->next[0]; i != 0; i = list->next[i])
    {
        int n = list->next[i];
        if (n != 0)
            fprintf(f, "  n%d -> n%d [color=\"#0050C0\", constraint=false];\n", i, n);
        else
            fprintf(f, "  n%d -> TAIL [color=\"#0050C0\", constraint=false];\n", i);
    }


    int freetop = (list->free_head != NIL) ? list->free_head : 0;

    fprintf(f, "  FREE_HEAD [shape=Mrecord, label=\"free_head\"];\n");
    fprintf(f, "  { rank=same; FREE_HEAD; n%d; }\n", freetop);
    fprintf(f, "  FREE_HEAD -> n%d [style=invis];\n", freetop);
    if (list->free_head != NIL)
        fprintf(f, "  FREE_HEAD -> n%d [color=\"#463f40\", style=bold, constraint=false];\n", list->free_head);

    for (int i = list->free_head; i != NIL; i = list->next[i])
    {
        int n = list->next[i];
        if (n != NIL)
            fprintf(f, "  n%d -> n%d [style=dashed, color=\"#808080\", constraint=false];\n", i, n);
    }


    fprintf(f, "}\n");
    fclose(f);
}

#endif
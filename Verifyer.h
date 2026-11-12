#ifndef VERIFYER_H
#define VERIFYER_H

#include <stdio.h>
#include "List.h"

void DotDump(const List* list, const char* filename);

void HtmlBegin(const char* filename);
void HtmlEnd  (const char* filename);

void HtmlDump_(const List* list, const char* png, const char* title,
               const char* html, const char* file, int line, const char* func);

#define HtmlDump(list, png, title, html) \
    HtmlDump_((list), (png), (title), (html), __FILE__, __LINE__, __func__)

Errors ListVerifyer(const List* list)
{
    int state[CAPACITY] = {};

    if (list->next[0] != 0 &&
        (list->next[0] < 1 || list->next[0] >= CAPACITY)) {
        fprintf(stderr, "FAIL 1: next[0]=%d nevaliden\n", list->next[0]);
        return ZERO_NEXT;
    }

    if (list->prev[0] != 0 &&
        (list->prev[0] < 1 || list->prev[0] >= CAPACITY)) {
        fprintf(stderr, "FAIL 2: prev[0]=%d nevaliden\n", list->prev[0]);
        return ZERO_PREV;
    }

    int count_forward = 0;
    int i = list->next[0];
    int prev_i = 0;

    while (i != 0) {
        if (i < 1 || i >= CAPACITY) {
            fprintf(stderr, "FAIL 3: next-obhod: index %d out of range\n", i);
            return FORWARD;
        }
        
        if (count_forward >= CAPACITY)
        {
            fprintf(stderr, "FAIL 3: loop");
            return FORWARD;
        }

        if (state[i] != 0)
        {
            fprintf(stderr, "FAIL 3: next-obhod: loop at node %d\n", i);
            HtmlDump(list, "error.png", "ERROR: LOOP", "history.html");
            return FORWARD;
        }

        state[i] = 1;
        count_forward++;

        if (list->prev[i] != prev_i) {
            fprintf(stderr, "FAIL 5a: prev[%d]=%d, expected %d\n",
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
            fprintf(stderr, "FAIL 3b: prev-obhod: index %d out of range\n", j);
            return BACKWARD;
        }

        if (count_back >= CAPACITY)
        {
            fprintf(stderr, "FAIL 4: loop in back obhod");
            return BACKWARD;
        }

        if (state[j] != 1) {
            fprintf(stderr, "FAIL 3b: prev-obhod: node %d was musor dropped\n", j);
            return BACKWARD;
        }
        count_back++;

        if (list->next[j] != next_j) {
            fprintf(stderr, "FAIL 5b: next[%d]=%d, expected %d\n",
                    j, list->next[j], next_j);
            return WRONG_PREV_NEXT;
        }
        next_j = j;
        j = list->prev[j];
    }

    if (count_forward != count_back) {
        fprintf(stderr, "FAIL 3c: forward %d nodes, back %d\n",
                count_forward, count_back);
        return FORWARD;
    }

    if (count_forward == 0) {
        if (list->next[0] != 0 || list->prev[0] != 0) {
            fprintf(stderr, "FAIL 6: empty list but next[0]=%d prev[0]=%d\n",
                    list->next[0], list->prev[0]);
            return EMPTY_ERROR;
        }
    }

    if (list->prev[0] != 0 && list->next[list->prev[0]] != 0) {
        fprintf(stderr, "FAIL 7: prev[0]=%d but next[%d]=%d (ne tail)\n",
                list->prev[0], list->prev[0], list->next[list->prev[0]]);
        return TAIL;
    }

    if (list->next[0] != 0 && list->prev[list->next[0]] != 0) {
        fprintf(stderr, "FAIL 8: next[0]=%d, но prev[%d]=%d (ne head)\n",
                list->next[0], list->next[0], list->prev[list->next[0]]);
        return HEAD;
    }

    if (list->free_head != NIL &&
        (list->free_head < 1 || list->free_head >= CAPACITY)) {
        fprintf(stderr, "FAIL 9: free_head=%d nevaliden\n", list->free_head);
        return FREE_HEAD;
    }
    if (list->free_head != NIL && state[list->free_head] != 0) {
        fprintf(stderr, "FAIL 9: free_head=%d in list\n", list->free_head);
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
    fprintf(f, "  splines=polyline;\n");
    fprintf(f, "  node [fontname=\"monospace\", fontsize=10, shape=Mrecord];\n");
    fprintf(f, "  edge [fontname=\"monospace\", fontsize=9];\n\n");

    for (int i = 0; i < CAPACITY; i++)
    {
        const char* fill;
        const char* style;
        if (i == 0) {
            fill = "#c10aee"; style = "filled";
        } else if (list->prev[i] == NIL) {
            fill = "#a6ff92"; style = "filled,dashed";
        } else {
            fill = "#a1a3fb"; style = "filled";
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

    if (list->next[0] != 0)
        fprintf(f, "  n0 -> n%d [color=\"#0050C0\", penwidth=2, constraint=false];\n", list->next[0]);

                

    int visited[CAPACITY] = {};
    int cur = list->next[0];
    int steps = 0;
    while (cur != 0 && steps < CAPACITY)
    {
        if (visited[cur])
        {
            fprintf(f, "  ERROR [shape=Mrecord, style = filled, fillcolor = \"#f80404\", label=\"ERROR | LOOP\"];\n");
            fprintf(f, "  n%d:e -> ERROR:w [color=red, penwidth=3, label=\"ERROR\"];\n", cur);
            break;
        }
        visited[cur] = 1;

        int n = list->next[cur];
        if (n != 0)
            fprintf(f, "  n%d:e -> n%d:w [color=\"#0050C0\", constraint=false];\n", cur, n);
        else
        {
            fprintf(f, "  n%d:s -> TAIL:n [color=\"#0050C0\", constraint=false];\n", cur);
            fprintf(f, "  n%d:s -> n0:n [color=\"#0050C0\", constraint=false, style=dashed];\n", cur);
        }
        cur = n;
        steps++;
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

void HtmlBegin(const char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f) { fprintf(stderr, "fopen\n"); return; }
    fprintf(f, "<html><body>\n");
    fclose(f);
}

void HtmlEnd(const char* filename)
{
    FILE* f = fopen(filename, "a");
    if (!f) { fprintf(stderr, "fopen\n"); return; }
    fprintf(f, "</body></html>\n");
    fclose(f);
}

void HtmlDump_(const List* list, const char* png, const char* title, const char* html, const char* file, int line, const char* func)
{
    FILE* f = fopen(html, "a");
    if (!f) { fprintf(stderr, "fopen\n"); return; }

    DotDump(list, "list.dot");

    char cmd[128] = {};
    snprintf(cmd, sizeof(cmd), "dot -Tpng list.dot -o %s", png);
    system(cmd);

    fprintf(f, "<h2>Called from: %s:%d in %s()</p>\n", file, line, func);
    fprintf(f, "<h2>%s</h2>\n", title);
    fprintf(f, "<img src=\"%s\"><br>\n", png);

    fprintf(f, "<table border=\"1\">\n");
    fprintf(f, "<tr><th>idx</th><th>data</th><th>next</th><th>prev</th></tr>\n");

    for (int i = 0; i < CAPACITY; i++)
    {
        fprintf(f, "<tr><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",
                i, list->data[i], list->next[i], list->prev[i]);
    }

    fprintf(f, "</table>\n<br>\n");
    fclose(f);
}

#endif
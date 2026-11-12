#include <stdio.h>
#include "List.h"
#include "Verifyer.h"

int CompareUp(int a, int b)
{
    return a - b;
}

int ListSort(List* list, int (*Comparator)(int a, int b))
{
    if (ListVerifyer(list) != OK) return -1;

    int swapped = 1;
    while (swapped) {
        swapped = 0;

        int p = 0;
        int a = list->next[p];
        while (a != 0 && list->next[a] != 0)
        {
            int b = list->next[a];

            if (Comparator(list->data[a], list->data[b]) > 0)
            {
                list->next[p] = b;
                list->prev[b] = p;

                list->next[a] = list->next[b];
                if (list->next[b] != 0)
                    list->prev[list->next[b]] = a;

                list->next[b] = a;
                list->prev[a] = b;

                if (list->prev[0] == b)
                    list->prev[0] = a;

                swapped = 1;

                p = b;
            }
            else
            {
                p = a;
                a = b;
            }
        }
    }

    return 0;
}

int main()
{
    List list = {0};
    InitList(&list);

    const char* html = "history.html";
    HtmlBegin(html);

    HtmlDump(&list, "begin.png", "step 0: empty", html);

    for (int i = 0; i < (CAPACITY - (rand() % CAPACITY)); i++)
    {
        PushAfter(&list, i, rand());
        char s[100] = {};
        sprintf(s, "step%d.png", i);
        HtmlDump(&list, s, "", html);
    }

    ListSort(&list, CompareUp);
    HtmlDump(&list, "sort.png", "sorted", html);

    HtmlEnd(html);
    return 0;
}




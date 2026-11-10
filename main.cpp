#include <stdio.h>
#include "List.h"
#include "Verifyer.h"

//TODO: sort function
int ListSort(List* list)
{
    if (ListVerifyer(list) != OK) return -1;
    
}

int main()
{
    List list = {0};
    InitList(&list);

    const char* html = "history.html";
    HtmlBegin(html);

    HtmlDump(&list, "step0.png", "step 0: empty", html);

    PushAfter(&list, 0, 10);
    HtmlDump(&list, "step1.png", "step 1: PushAfter(0, 10)", html);

    PushAfter(&list, 1, 20);
    HtmlDump(&list, "step2.png", "step 2: PushAfter(1, 20)", html);

    PushAfter(&list, 2, 30);
    HtmlDump(&list, "step3.png", "step 2: PushAfter(1, 20)", html);

    PushAfter(&list, 3, 40);
    HtmlDump(&list, "step4.png", "step 2: PushAfter(1, 20)", html);

    DeleteElem(&list, 3);
    HtmlDump(&list, "step5.png", "delete 3", html);

    //list.next[2] = 2;

    PushAfter(&list, 2, 200);
    HtmlDump(&list, "step6.png", "step 6: PushAfter(2, 200)", html);

    HtmlEnd(html);
    return 0;
}




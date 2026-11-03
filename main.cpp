#include <stdio.h>
#include "List.h"
#include "Verifyer.h"

int main()
{
    List list = {0};
    InitList(&list);
    
    for (int i = 0; i < 10; i++)
        PushAfter(&list, i, i+1);

    DeleteElem(&list, 5);
    DeleteElem(&list, 3);

    DotDump(&list, "step4.dot");
    return 0;
}




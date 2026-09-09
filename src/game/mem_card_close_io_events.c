#include "../types.h"
#include "mem_card.h"
#include "../psyq/libapi.h"

void MemCard_CloseIOEvents(void)
{
    long *item = gMemCard_aIOEventHandles;
    int count;
    EnterCriticalSection();
    count = 8;
    do {
        CloseEvent(*item++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

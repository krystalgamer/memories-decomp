#include "../types.h"
#include "../psyq/libapi.h"

extern long gMemCard_aIOEventHandles[];

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

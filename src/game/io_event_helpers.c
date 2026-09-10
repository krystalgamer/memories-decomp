#include "../types.h"
#include "mem_card.h"
#include "../psyq/libapi.h"
#include "io_event_helpers.h"

void MemCard_ClearIOEvents(long *handles)
{
    TestEvent(handles[0]);
    TestEvent(handles[1]);
    TestEvent(handles[2]);
    TestEvent(handles[3]);
    gMemCard_nIOResult = -1;
}

s32 MemCard_WaitIOEvent(long *handles, s32 once)
{
    do {
        if (TestEvent(handles[0]) == 1)
            return 0;
        if (TestEvent(handles[1]) == 1)
            return 1;
        if (TestEvent(handles[2]) == 1)
            return 2;
        if (TestEvent(handles[3]) == 1)
            return 3;
    } while (once == 0);
    return -1;
}

void MemCard_Init(long val)
{
    InitCARD(val);
    StartCARD();
    ChangeClearPAD(0);
    _bu_init();
}

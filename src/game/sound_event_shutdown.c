#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"

void func_8004B910(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    DisableEvent(D_8009B458->event_handle);
    CloseEvent(D_8009B458->event_handle);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

void func_8004B990(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

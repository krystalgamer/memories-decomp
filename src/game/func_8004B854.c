#include "../types.h"
#include "../psyq/libapi.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "sound_event_runtime.h"

void func_8004B854(void)
{
    long event;

    if (D_8009B458->event_guard)
        return;

    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    event = OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, SD_SequenceTimerCallback);
    D_8009B458->event_handle = event;
    EnableEvent(event);
    SetRCnt(RCntCNT2, 0xE000, RCntMdINTR);
    StartRCnt(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
    D_8009B458->event_guard = 0;
}

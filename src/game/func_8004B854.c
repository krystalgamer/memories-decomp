#include "../types.h"
#include "../psyq/libapi.h"
#include "sound.h"

extern long SD_SequenceTimerCallback(void);

void func_8004B854(void)
{
    long event;

    if (D_8009B458->event_guard)
        return;

    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    {
        register unsigned long descriptor asm("$4") = RCntCNT2;
        register long specification asm("$5") = EvSpINT;
        register long mode asm("$6") = EvMdINTR;
        register long (*callback)(void) asm("$7") = SD_SequenceTimerCallback;

        event = OpenEvent(descriptor, specification, mode, callback);
    }
    D_8009B458->event_handle = event;
    EnableEvent(event);
    SetRCnt(RCntCNT2, 0xE000, RCntMdINTR);
    StartRCnt(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
    D_8009B458->event_guard = 0;
}

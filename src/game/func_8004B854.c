#include "../types.h"
#include "../psyq/libapi.h"
#include "../unmatched.h"
#include "../game/sound.h"
#include "../game/sound_event_runtime.h"

void func_8004B854(void)
{
    long event;

    if (D_8009B458->event_guard)
        return;

    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    {
        unsigned long descriptor = RCntCNT2;
        long specification = EvSpINT;
        long mode = EvMdINTR;
        long (*callback)(void) = SD_SequenceTimerCallback;

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

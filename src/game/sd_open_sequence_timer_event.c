#include "../types.h"
#include "../psyq/libapi.h"
#define D_8009B458_IN_DATA
#include "sound.h"
#include "sound_event_runtime.h"

void SD_OpenSequenceTimerEvent(void)
{
    PSXLONG event;

    if (gSD_pSecondaryState->event_guard)
        return;

    gSD_pSecondaryState->event_guard = 1;
    EnterCriticalSection();
    StopRCnt(RCntCNT2);
    event = OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, SD_SequenceTimerCallback);
    gSD_pSecondaryState->event_handle = event;
    EnableEvent(event);
    SetRCnt(RCntCNT2, 0xE000, RCntMdINTR);
    StartRCnt(RCntCNT2);
    ExitCriticalSection();
    gSD_pSecondaryState->timer_update_inhibit = 0;
    gSD_pSecondaryState->event_guard = 0;
}

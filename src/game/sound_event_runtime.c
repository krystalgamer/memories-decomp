#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"

extern void SD_ProcessSequenceTracks(void);
extern void func_8004C84C(void);
extern void func_8004AAFC(void);

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

void func_8004B9E0(void)
{
    if (D_8009B458->flag_0501)
        return;
    D_8009B458->flag_0501 = 1;
    if (D_8009B458->flag_0502) {
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
        SD_ProcessSequenceTracks();
    }
    func_8004C84C();
    func_8004AAFC();
    D_8009B458->flag_0501 = 0;
}

#include "../types.h"
#include "../psyq/libapi.h"

#include "sound.h"

extern void func_80073A54(u32);
extern void func_800738A0(long);

void func_8004B910(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    func_80073A54(RCntCNT2);
    func_800738A0(D_8009B458->event_handle);
    CloseEvent(D_8009B458->event_handle);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

void func_8004B990(void)
{
    D_8009B458->event_guard = 1;
    EnterCriticalSection();
    func_80073A54(RCntCNT2);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

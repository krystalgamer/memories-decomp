#include "../types.h"

#include "sound.h"

extern int EnterCriticalSection(void);
extern void func_80073A54(u32);
extern void func_800738A0(void *);
extern void CloseEvent(void *);
extern void ExitCriticalSection(void);

void func_8004B910(void)
{
    D_8009B458->flag_0503 = 1;
    EnterCriticalSection();
    func_80073A54(0xF2000002);
    func_800738A0(D_8009B458->field_0504);
    CloseEvent(D_8009B458->field_0504);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

void func_8004B990(void)
{
    D_8009B458->flag_0503 = 1;
    EnterCriticalSection();
    func_80073A54(0xF2000002);
    ExitCriticalSection();
    D_8009B458->flag_0500 = 0;
}

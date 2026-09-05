#include "../types.h"

extern void *D_800F2AE0[];
extern int EnterCriticalSection(void);
extern void CloseEvent(void *);
extern void ExitCriticalSection(void);

void func_80043E68(void)
{
    void **item = D_800F2AE0;
    int count;
    EnterCriticalSection();
    count = 8;
    do {
        CloseEvent(*item++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

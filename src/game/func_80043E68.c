#include "../types.h"
#include "../psyq/libapi.h"

extern long D_800F2AE0[];

void func_80043E68(void)
{
    long *item = D_800F2AE0;
    int count;
    EnterCriticalSection();
    count = 8;
    do {
        CloseEvent(*item++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

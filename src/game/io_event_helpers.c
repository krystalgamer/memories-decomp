#include "../types.h"
#include "../psyq/libapi.h"

extern s32 gMemCard_nIOResult;

extern void func_8008B3E0(void);
extern void func_8008B470(void);

void func_80043D48(long *items)
{
    TestEvent(items[0]);
    TestEvent(items[1]);
    TestEvent(items[2]);
    TestEvent(items[3]);
    gMemCard_nIOResult = -1;
}

s32 func_80043DA0(long *items, s32 stop)
{
    do {
        if (TestEvent(items[0]) == 1)
            return 0;
        if (TestEvent(items[1]) == 1)
            return 1;
        if (TestEvent(items[2]) == 1)
            return 2;
        if (TestEvent(items[3]) == 1)
            return 3;
    } while (stop == 0);
    return -1;
}

void func_80043E30(void)
{
    func_8008B3E0();
    func_8008B470();
    ChangeClearPAD(0);
    _bu_init();
}

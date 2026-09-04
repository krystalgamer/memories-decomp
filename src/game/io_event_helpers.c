#include "../types.h"

extern s32 D_8009B450;

extern s32 func_80073880(void *);
extern void func_80073840(void);
extern void func_80073940(s32);
extern void func_8008B3E0(void);
extern void func_8008B470(void);

void func_80043D48(void **items)
{
    func_80073880(items[0]);
    func_80073880(items[1]);
    func_80073880(items[2]);
    func_80073880(items[3]);
    D_8009B450 = -1;
}

s32 func_80043DA0(void **items, s32 stop)
{
    do {
        if (func_80073880(items[0]) == 1)
            return 0;
        if (func_80073880(items[1]) == 1)
            return 1;
        if (func_80073880(items[2]) == 1)
            return 2;
        if (func_80073880(items[3]) == 1)
            return 3;
    } while (stop == 0);
    return -1;
}

void func_80043E30(void)
{
    func_8008B3E0();
    func_8008B470();
    func_80073940(0);
    func_80073840();
}

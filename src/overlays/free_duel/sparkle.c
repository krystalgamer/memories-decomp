#include "../../types.h"

extern u8 D_801AF000[];
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, int);
extern void func_800428A8(void *, int, int, int, int, int, int, int, int);
extern void func_800428EC(void *, int);

u8 *FreeDuel_SpawnSparkle(void)
{
    u8 *x;

    x = func_800400AC(func_8004002C(), 2);
    func_800428A8(x, 0, 0, 0, 0, 3, 0x11, 3, D_801AF000);
    x[0x5F] = 0x80;
    *(s32 *)(x + 0x48) = 0x180018;
    func_800428EC(x, 5);
    *(u16 *)(x + 8) = *(u16 *)(x + 8) | 0x20;
    return x;
}

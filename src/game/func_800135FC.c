#include "../types.h"

extern u8 D_800F2848[];
extern int rcos(int);
extern int rsin(int);
extern void func_800134E0();

void func_800135FC(void)
{
    u8 *record = D_800F2848;
    s32 distance = -*(s16 *)record;
    s32 x = distance * rcos(*(s16 *)(record + 2)) / 4096;
    s32 z = distance * rsin(*(s16 *)(record + 2)) / 4096;
    s32 y = x * rsin(*(s16 *)(record + 4) + 1024) / 4096;
    s32 w = x * rcos(*(s16 *)(record + 4) + 1024) / 4096;

    x = w;
    func_800134E0(record, x, y, z);
}

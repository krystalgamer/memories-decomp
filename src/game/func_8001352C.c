#include "../types.h"

extern u8 D_800F2848[];
extern int rcos(int);
extern int rsin(int);
extern void func_800134E0(u8 *, s32, s32, s32);

void func_8001352C(void) {
    u8 *p = D_800F2848;
    s32 n = -*(s16 *)D_800F2848;
    s32 x = n * rcos(*(s16 *)(p + 4)) / 4096;
    s32 z = n * rsin(*(s16 *)(p + 4)) / 4096;
    s32 y = x * rsin(*(s16 *)(p + 2)) / 4096;
    s32 w = x * rcos(*(s16 *)(p + 2)) / 4096;

    x = w;
    func_800134E0(p, x, z, y);
}

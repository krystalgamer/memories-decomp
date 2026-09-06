#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"

extern u8 D_800F2848[];
extern void func_800134E0(u8 *, s32, s32, s32);

void func_8001352C(void) {
    u8 *p = D_800F2848;
    s32 n = -*(s16 *)D_800F2848;
    s32 x = n * rcos(*(s16 *)(p + 4)) / MODEL_FIXED_ONE;
    s32 z = n * rsin(*(s16 *)(p + 4)) / MODEL_FIXED_ONE;
    s32 y = x * rsin(*(s16 *)(p + 2)) / MODEL_FIXED_ONE;
    s32 w = x * rcos(*(s16 *)(p + 2)) / MODEL_FIXED_ONE;

    x = w;
    func_800134E0(p, x, z, y);
}

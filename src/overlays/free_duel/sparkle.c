#include "../../types.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_helpers.h"

extern u8 D_801AF000[];

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

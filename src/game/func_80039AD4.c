#include "../types.h"
#include "duel_effect.h"
#include "func_80039AD4.h"

extern u8 D_800EAF08[DUEL_EFFECT_OCCUPANCY_COUNT];
extern s32 D_8009B330;
void func_80039AD4(u8 *object)
{
    D_800EAF08[object[0x10]] = 0;
    object[0x11] = 0;
    D_8009B330 = 1;
}

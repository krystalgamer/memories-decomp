#include "../types.h"
#include "duel_effect.h"
#include "func_80039AD4.h"

void func_80039AD4(DuelEffectChannel *object)
{
    D_800EAF08[((u8 *)object)[0x10]] = 0;
    ((u8 *)object)[0x11] = 0;
    D_8009B330 = 1;
}

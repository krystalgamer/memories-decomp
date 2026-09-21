#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_object_commands.h"

void func_800389C4(DuelEffectChannel *value)
{
    value->flags_34 &= (u16)~8;
}

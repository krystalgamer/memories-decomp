#include "../types.h"
#include "duel_effect_resource_record.h"
#include "func_80029574.h"

void func_80029574(int index)
{
    DuelEffectResourceRecord *entry = &D_800EA0E8[index];

    entry->object_04 = 0;
    entry->object_00 = 0;
}

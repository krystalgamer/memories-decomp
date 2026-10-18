#include "../types.h"
#include "duel_effect_resource_record.h"
#include "func_80029574.h"

void DuelEffect_ClearResourceObjectPointers(int index)
{
    DuelEffectResourceRecord *entry = &D_800EA0E8[index];

    entry->secondary_object = 0;
    entry->primary_object = 0;
}

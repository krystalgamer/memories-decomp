#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_occupancy.h"
#include "func_80035A64.h"

void func_80035A64(void)
{
    s32 i = 4;
    DuelEffectChannel *entry = D_800EB0F8;

    do {
        entry->flags_34 = 0;
        entry->field_30 = 0;
        entry->field_2C = 0;
        entry->field_28 = 0;
        entry++;
    } while (--i != 0);
    DuelEffect_ResetOccupancy();
    DuelEffect_ResetEntryMarkers();
}

#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_control.h"

#define DUEL_EFFECT_ENTRY_FROM_FIELD_13(field) \
    ((DuelEffectEntry *)((field) - 0x13))
#define DUEL_EFFECT_ENTRY_FROM_FIELD_15(field) \
    ((DuelEffectEntry *)((field) - 0x15))

/* Starting from the record's entry range, scans up to range_count_5C entries;
   returns 1 on the first entry with DUEL_EFFECT_ENTRY_FLAG_ACTIVE set and
   field_13 nonzero, 0
   if that flag clears, the count runs out, or the range is empty. */
int DuelEffect_HasActiveEntry(DuelEffectChannel *a0) {
    int v0;
    int count;
    u8 *v1;
    v0 = a0->range_start_5C;
    count = a0->range_count_5E;
    v1 = (u8 *)&D_800EB288[v0];
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = v1 + 19;
loop:
    v0 = DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->flags_11 &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
    if (v0 == 0) {
        return v0;
    }
    if (DUEL_EFFECT_ENTRY_FROM_FIELD_13(v1)->field_13 != 0) {
        return 1;
    }
    count = count - 1;
    v1 = v1 + sizeof(DuelEffectEntry);
    if (count != 0) {
        goto loop;
    }
ret_zero_a:
    return 0;
}

/* Starting from the record's entry range, walks up to range_count_5E entries;
   for each active entry, writes a1 to field_13 and a2 to field_15,
   stopping at the first entry with that flag clear or when the count runs
   out. */
void func_800373C8(DuelEffectChannel *a0, u8 a1, u8 a2) {
    int v0;
    int count;
    u8 *v1;

    v0 = a0->range_start_5C;
    count = a0->range_count_5E;
    v1 = (u8 *)&D_800EB288[v0];
    if (count == 0) {
        return;
    }
    v1 = v1 + 21;
loop:
    if ((DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->flags_11 &
         DUEL_EFFECT_ENTRY_FLAG_ACTIVE) == 0) {
        return;
    }
    count = count - 1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->field_13 = a1;
    DUEL_EFFECT_ENTRY_FROM_FIELD_15(v1)->field_15 = a2;
    v1 = v1 + sizeof(DuelEffectEntry);
    if (count != 0) {
        goto loop;
    }
}

void Text_CompletePageAdvance(DuelEffectChannel *object)
{
    u8 state = object->state_51;
    DuelEffectEntry *entry;

    if ((state & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        object->state_51 = state | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        func_800373C8(object, 2, 0);
        return;
    }

    entry = &D_800EB288[object->range_start_5C];
    if (entry->flags_11 & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        return;
    }

    object->field_56 = 0;
    object->field_38 = 0;
    object->field_3A = 0;
    object->state_51 = 0;
    object->field_62 = 0;
}

#include "../types.h"
#include "duel_effect.h"

/* Starting from the record's entry range, scans up to range_count_5C entries;
   returns 1 on the first entry with flags_11&0x80 set and field_13 nonzero, 0
   if that flag clears, the count runs out, or the range is empty. */
int DuelEffect_HasActiveEntry(DuelEffectChannel *a0) {
    int v0;
    int count;
    u8 *v1;
    v0 = a0->range_start_5C;
    count = a0->range_count_5E;
    v1 = (u8 *)D_800EB288 + v0 * sizeof(DuelEffectEntry);
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = v1 + 19;
loop:
    v0 = v1[-2] & 0x80;
    if (v0 == 0) {
        return v0;
    }
    if (v1[0] != 0) {
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
   for each with flags_11&0x80 set, writes a1 to field_13 and a2 to field_15,
   stopping at the first entry with that flag clear or when the count runs
   out. */
void func_800373C8(DuelEffectChannel *a0, u8 a1, u8 a2) {
    int v0;
    int count;
    u8 *v1;

    v0 = a0->range_start_5C;
    count = a0->range_count_5E;
    v1 = (u8 *)D_800EB288 + v0 * sizeof(DuelEffectEntry);
    if (count == 0) {
        return;
    }
    v1 = v1 + 21;
loop:
    if ((v1[-4] & 0x80) == 0) {
        return;
    }
    count = count - 1;
    v1[-2] = a1;
    v1[0] = a2;
    v1 = v1 + sizeof(DuelEffectEntry);
    if (count != 0) {
        goto loop;
    }
}

void func_8003741C(u8 *object)
{
    u8 state = object[0x51];
    DuelEffectEntry *entry;

    if ((state & 0x80) == 0) {
        object[0x51] = state | 0x80;
        func_800373C8((DuelEffectChannel *)object, 2, 0);
        return;
    }

    entry = &D_800EB288[*(u16 *)(object + 0x5C)];
    if (entry->flags_11 & 0x80) {
        return;
    }

    object[0x56] = 0;
    *(u16 *)(object + 0x38) = 0;
    *(u16 *)(object + 0x3A) = 0;
    object[0x51] = 0;
    object[0x62] = 0;
}

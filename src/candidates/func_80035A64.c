/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * 2 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_80035A64.c.
 */
#include "../types.h"
#include "../game/duel_effect.h"
#include "../game/duel_effect_entry_occupancy.h"
#include "../game/func_80035A64.h"

void func_80035A64(void) {
    int i = 4;
    DuelEffectChannel *entry = D_800EB0F8;
    __asm__ volatile("" : "+r"(entry));
    do {
        entry->flags_34 = 0;
        entry->field_30 = 0;
        entry->field_2C = 0;
        entry->field_28 = 0;
        __asm__ volatile("" ::: "memory");
        i--;
        entry++;
    } while (i != 0);
    DuelEffect_ResetOccupancy();
    DuelEffect_ResetEntryMarkers();
}

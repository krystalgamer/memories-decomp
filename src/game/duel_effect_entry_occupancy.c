#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_occupancy.h"
#include "../unmatched.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_CLEAR_DUEL_EFFECT_OCCUPANCY)
void func_80035CA8(int value)
{
    int index;
    u8 *entry = tent_DuelEffectOccupancy;
    value++;
    for (
        index = 0;
        index < DUEL_EFFECT_OCCUPANCY_COUNT;
        index++, entry++
    ) {
        if (*entry == value) {
            *entry = 0;
        }
    }
}
#endif

/* Clears the tent_DuelEffectOccupancy occupancy table (see func_80035CA8) and resets its
   scan cursor to (0,0). */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DUEL_EFFECT_RESET_OCCUPANCY)
void DuelEffect_ResetOccupancy(void) {
    u8 *v0;
    int v1;
    v0 = tent_DuelEffectOccupancy;
    v1 = DUEL_EFFECT_OCCUPANCY_COUNT - 1;
    do {
        *v0 = 0;
        v1 = v1 - 1;
        v0 = v0 + 1;
    } while (v1 >= 0);
    D_8009B324 = 0;
    D_8009B325 = 0;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DUEL_EFFECT_FIND_OCCUPANCY)
s32 func_80035D10(void)
{
    s32 col = D_8009B324;
    s32 row = D_8009B325;
    s32 count = 0x100;
    do {
        s32 index = row * 16 + col;
        if (tent_DuelEffectOccupancy[index] == 0)
            return index;
        col++;
        if ((col & 3) == 0) {
            row++;
            if (row >= 0xF)
                row = 0x10;
            if ((row & 3) == 0)
                row -= 4;
            else
                col -= 4;
            if (col >= 0x10) {
                row += 4;
                col = 0;
                if (row >= 0x10)
                    row = 0;
            }
        }
        count--;
    } while (count != 0);
    return -1;
}
#endif

/* Clears field17 on the first (only) entry whose field18 equals a0+1,
   scanning 620 entries. */
#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DUEL_EFFECT_CLEAR_MATCHING_MARKER)
void DuelEffect_ClearMatchingMarker(int a0) {
#ifdef VERSION_JAPAN
    JapaneseDuelEffectEntry *v1;
    int a1;
    u8 v0;
    a1 = JAPANESE_DUEL_EFFECT_ENTRY_COUNT;
    a0 = a0 + 1;
    v1 = (JapaneseDuelEffectEntry *)tent_DuelEffectEntries;
    do {
        v0 = v1->field_12;
        if (v0 == a0) {
            v1->flags_11 = 0;
        }
        a1 -= 1;
        v1 += 1;
    } while (a1 != 0);
#else
    DuelEffectEntry *v1;
    int a1;
    u8 v0;
    a1 = DUEL_EFFECT_ENTRY_COUNT;
    a0 = a0 + 1;
    v1 = tent_DuelEffectEntries;
    do {
        v0 = v1->field_12;
        if (v0 == a0) {
            v1->flags_11 = 0;
        }
        a1 -= 1;
        v1 += 1;
    } while (a1 != 0);
#endif
}
#endif

#ifdef VERSION_JAPAN
#define DUEL_EFFECT_MARKER_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT JAPANESE_DUEL_EFFECT_ENTRY_COUNT
#else
#define DUEL_EFFECT_MARKER_ENTRY_TYPE DuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT DUEL_EFFECT_ENTRY_COUNT
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DUEL_EFFECT_RESET_ENTRY_MARKERS)
/* Clears the marker byte in every entry; the North American layout also
   clears field_18. */
void DuelEffect_ResetEntryMarkers(void) {
    DUEL_EFFECT_MARKER_ENTRY_TYPE *v0;
    int v1;
    /* Old GCC preserves the regional initialization order in the output. */
#ifdef VERSION_JAPAN
    v0 = (DUEL_EFFECT_MARKER_ENTRY_TYPE *)tent_DuelEffectEntries;
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
#else
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
    v0 = tent_DuelEffectEntries;
#endif
    for (; v1 != 0; v1 = v1 - 1) {
        v0->flags_11 = 0;
#ifndef VERSION_JAPAN
        v0->field_18 = 0;
#endif
        v0 = v0 + 1;
    }
}
#endif

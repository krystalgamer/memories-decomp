#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_occupancy.h"
#include "../unmatched.h"

#ifndef VERSION_EUROPE
void DuelEffect_ClearOccupancyValue(int value)
{
    int index;
    u8 *entry = gDuelEffect_aOccupancyValues;
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

/* Clears the gDuelEffect_aOccupancyValues occupancy table (see DuelEffect_ClearOccupancyValue) and resets its
   scan cursor to (0,0). */
#ifndef VERSION_EUROPE
void DuelEffect_ResetOccupancy(void) {
    u8 *v0;
    int v1;
    v0 = gDuelEffect_aOccupancyValues;
    v1 = DUEL_EFFECT_OCCUPANCY_COUNT - 1;
    do {
        *v0 = 0;
        v1 = v1 - 1;
        v0 = v0 + 1;
    } while (v1 >= 0);
    gDuelEffect_OccupancyStartColumn = 0;
    gDuelEffect_OccupancyStartRow = 0;
}
#endif

#ifndef VERSION_EUROPE
s32 DuelEffect_FindFreeOccupancySlot(void)
{
    s32 col = gDuelEffect_OccupancyStartColumn;
    s32 row = gDuelEffect_OccupancyStartRow;
    s32 count = 0x100;
    do {
        s32 index = row * 16 + col;
        if (gDuelEffect_aOccupancyValues[index] == 0)
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

#ifdef VERSION_JAPAN
#define DUEL_EFFECT_MARKER_ENTRY_TYPE JapaneseDuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT JAPANESE_DUEL_EFFECT_ENTRY_COUNT
#elif defined(VERSION_EUROPE)
#define DUEL_EFFECT_MARKER_ENTRY_TYPE EuropeanDuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT EUROPEAN_DUEL_EFFECT_ENTRY_COUNT
#else
#define DUEL_EFFECT_MARKER_ENTRY_TYPE DuelEffectEntry
#define DUEL_EFFECT_MARKER_ENTRY_COUNT DUEL_EFFECT_ENTRY_COUNT
#endif

/* Clears the marker on each regional entry whose owner_channel_tag equals a0 + 1. */
void DuelEffect_ClearMatchingMarker(int a0) {
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    DUEL_EFFECT_MARKER_ENTRY_TYPE *v1;
    int a1;
    u8 v0;
    a1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
    a0 = a0 + 1;
    v1 = (DUEL_EFFECT_MARKER_ENTRY_TYPE *)gDuelEffect_aEntries;
    do {
        v0 = v1->owner_channel_tag;
        if (v0 == a0) {
            v1->entry_flags = 0;
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
    v1 = gDuelEffect_aEntries;
    do {
        v0 = v1->owner_channel_tag;
        if (v0 == a0) {
            v1->entry_flags = 0;
        }
        a1 -= 1;
        v1 += 1;
    } while (a1 != 0);
#endif
}

/* Clears the marker byte in every entry; the North American layout also
   clears layout_selector, the Japanese and European ones do not. */
void DuelEffect_ResetEntryMarkers(void) {
    DUEL_EFFECT_MARKER_ENTRY_TYPE *v0;
    int v1;
    /* Old GCC preserves the regional initialization order in the output. */
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
    v0 = (DUEL_EFFECT_MARKER_ENTRY_TYPE *)gDuelEffect_aEntries;
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
#else
    v1 = DUEL_EFFECT_MARKER_ENTRY_COUNT;
    v0 = gDuelEffect_aEntries;
#endif
    for (; v1 != 0; v1 = v1 - 1) {
        v0->entry_flags = 0;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        v0->layout_selector = 0;
#endif
        v0 = v0 + 1;
    }
}

#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_ranges.h"

/* Initialized data at 0x80090E58: one start per duel-effect channel followed
   by the common end, then one retail padding halfword. */
u16 gDuelEffect_awEntryRangeBoundaries[] = {
    0,
    255,
    415,
    575,
    DUEL_EFFECT_ENTRY_COUNT,
    0,
};

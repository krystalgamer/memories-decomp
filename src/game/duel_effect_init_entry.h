#ifndef MEMORIES_DECOMP_DUEL_EFFECT_INIT_ENTRY_H
#define MEMORIES_DECOMP_DUEL_EFFECT_INIT_ENTRY_H

#include "duel_effect.h"

/* Initialises channel `index` for string `value`, ORs 0x8000 into its flags,
 * and takes the entry range for the channel from consecutive
 * gDuelEffect_awEntryRangeBoundaries values. Returns the channel it
 * initialised. */
DuelEffectChannel *DuelEffect_InitEntry(s32 index, s32 value, s32 flags);

#endif

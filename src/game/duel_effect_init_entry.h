#ifndef MEMORIES_DECOMP_DUEL_EFFECT_INIT_ENTRY_H
#define MEMORIES_DECOMP_DUEL_EFFECT_INIT_ENTRY_H

#include "duel_effect.h"

/* Initialises channel `index` for string `value`, ORs 0x8000 into its flags,
 * and takes its range from consecutive gDuelEffect_awEntryRangeBoundaries
 * values. Japanese channels have a 0x60-byte stride and byte-sized range
 * fields; the returned pointer still names the selected channel. */
DuelEffectChannel *DuelEffect_InitEntry(s32 index, s32 value, s32 flags);

#endif

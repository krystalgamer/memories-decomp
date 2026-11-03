#ifndef GAME_DUEL_EFFECT_ENTRY_STATE_H
#define GAME_DUEL_EFFECT_ENTRY_STATE_H

#include "../types.h"
#include "duel_effect.h"

/* The entry record and count differ by build; the units that define or walk
 * the regional records select them before including this header. */
#ifndef DUEL_EFFECT_ENTRY_STORAGE_TYPE
#define DUEL_EFFECT_ENTRY_STORAGE_TYPE DuelEffectEntry
#define DUEL_EFFECT_ENTRY_STORAGE_COUNT DUEL_EFFECT_ENTRY_COUNT
#endif
extern DUEL_EFFECT_ENTRY_STORAGE_TYPE
    gDuelEffect_aEntries[DUEL_EFFECT_ENTRY_STORAGE_COUNT];
extern u8 gDuelEffect_aOccupancyValues[DUEL_EFFECT_OCCUPANCY_COUNT];
extern s32 gDuelEffect_nCompactionRequested;

#endif

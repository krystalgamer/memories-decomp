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

/* The shared compaction-request word. DuelEffect_ProcessEntries clears it
 * before invoking entry handlers; DisplayObjectFade_ReleaseChannel raises it.
 * A nonzero request after that walk triggers stable active-entry compaction,
 * not a handler-walk restart or a queued next-frame guarantee.
 * See notes/entry-lifecycle-state.md. */
extern s32 gDuelEffect_nCompactionRequested;

#endif

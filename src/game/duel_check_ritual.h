#ifndef MEMORIES_DECOMP_DUEL_CHECK_RITUAL_H
#define MEMORIES_DECOMP_DUEL_CHECK_RITUAL_H

#include "../types.h"
#include "card_constants.h"

/* Optional ritual-match output: the three matched tribute display objects,
 * followed by a word the function clears. */
typedef struct {
    void *tribute_objects[DUEL_RITUAL_TRIBUTE_COUNT];
    s32 field_0C;
} DuelRitualResult;

/* Returns the u16 result card id widened to s32. Declaring a narrow return at
 * the callers adds a zero-extension instruction that retail does not have. */
s32 Duel_CheckRitual(DuelRitualResult *out, s32 ritual_id);

#endif

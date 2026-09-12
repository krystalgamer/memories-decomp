#ifndef MEMORIES_DECOMP_DUEL_EFFECT_STATE_LATCH_H
#define MEMORIES_DECOMP_DUEL_EFFECT_STATE_LATCH_H

#include "../types.h"

/* Marks the active state handler initialized. The first call sets bit 0x80
 * and returns zero; later calls return one. */
int DuelEffect_MarkStateInitialized(void);

#endif

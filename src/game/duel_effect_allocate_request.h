#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ALLOCATE_REQUEST_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ALLOCATE_REQUEST_H

#include "../types.h"

/* Allocates a request for the given effect id, marks the pool active, and
 * returns its byte-oriented payload view, or 0 when the pool is full. */
u8 *DuelEffect_AllocateRequest(s32 id);

#endif

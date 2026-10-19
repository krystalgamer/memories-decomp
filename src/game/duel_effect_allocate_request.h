#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ALLOCATE_REQUEST_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ALLOCATE_REQUEST_H

#include "../types.h"

struct DuelEffectRequest;

/* Initializes a free request and raises its ACTIVE flag, or returns 0 when
 * full. The separate pool-status latch is raised by DuelEffect_CreateRequest. */
struct DuelEffectRequest *DuelEffect_AllocateRequest(s32 id);

#endif

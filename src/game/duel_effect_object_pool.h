#ifndef MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_POOL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_POOL_H

#include "duel_effect_request.h"

/* Tests workspace byte offset +0x24F, the card-quantity bias for one-based
 * IDs. No caller establishes a bounded ID contract, so keep the byte offset. */
s32 func_8002C570(s32 offset);
void DuelEffect_ResetRequestPool(void);

extern u8 gDuelEffect_bRequestFinished;
#ifdef D_8009B264_VISIBLE
extern DuelEffectRequest *G32 gDuelEffect_pActiveRequest;
#endif

#endif

#ifndef MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_POOL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_POOL_H

#include "duel_effect_request.h"

void DuelEffect_ResetRequestPool(void);

extern u8 gDuelEffect_bRequestFinished;
extern DuelEffectRequest *gDuelEffect_pActiveRequest;

#endif

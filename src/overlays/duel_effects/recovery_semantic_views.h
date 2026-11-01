#ifndef MEMORIES_DECOMP_DUEL_RECOVERY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_RECOVERY_SEMANTIC_VIEWS_H

#include "effect_5.h"
#include "effect_9.h"

extern void DuelEffect_RunLifePointRecovery(void *work, s32 phase);
extern void DuelEffect_RunInvertedLifePointRecovery(void *work, s32 phase);

extern DuelEffect5Config gDuelEffect_LifePointRecoveryConfigs[10];
extern DuelEffect9Config gDuelEffect_InvertedLifePointRecoveryConfigs[5];
extern VECTOR gDuelEffect_LifePointRecoveryInitialScale;
extern VECTOR gDuelEffect_InvertedLifePointRecoveryInitialScale;

#endif

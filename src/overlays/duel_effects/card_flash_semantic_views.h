#ifndef MEMORIES_DECOMP_DUEL_CARD_FLASH_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_CARD_FLASH_SEMANTIC_VIEWS_H

#include "effect_4.h"
#include "effect_19.h"

extern void DuelEffect_RunCardFlashBurst(void *work, s32 phase);
extern void DuelEffect_RunExpandingRingBurst(void *work, s32 phase);
extern DuelEffect4Config gDuelEffect_CardFlashBurstConfigs[2];
extern VECTOR gDuelEffect_CardFlashBurstInitialScale;
extern VECTOR gDuelEffect_ExpandingRingBurstInitialScale;

#endif

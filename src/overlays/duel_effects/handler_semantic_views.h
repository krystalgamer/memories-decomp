#ifndef MEMORIES_DECOMP_DUEL_HANDLER_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_HANDLER_SEMANTIC_VIEWS_H

#include "../../types.h"

extern void DuelEffect_RunFusion(void *work, s32 phase);
extern void DuelEffect_RunAttackSlash(void *work, s32 phase, s16 damage);
extern void DuelEffect_RunDestroyFlames(void *work, s32 phase);
extern void DuelEffect_RunTerrainChange(void *work, s32 phase);
extern void DuelEffect_RunGuardianStarBonus(void *work, s32 phase);
extern void DuelEffect_RunBoardDestruction(void *work, s32 phase);

#endif

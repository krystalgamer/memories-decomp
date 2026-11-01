#ifndef MEMORIES_DECOMP_DUEL_STAT_PENALTY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_STAT_PENALTY_SEMANTIC_VIEWS_H

#include "effect_13.h"

extern void DuelEffect_RunStatPenalty(void *work, s32 phase, s16 number);
extern void DuelEffect_InitJaggedColumnVertices(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices);

extern DuelEffect13Config gDuelEffect_StatPenaltyConfigs[4];
extern VECTOR gDuelEffect_StatPenaltyInitialScale;
extern GsIMAGE gDuelEffect_StatPenaltyTextureImages[21];

#endif

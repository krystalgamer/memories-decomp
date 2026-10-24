#ifndef MEMORIES_DECOMP_DUEL_STAT_PENALTY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_STAT_PENALTY_SEMANTIC_VIEWS_H

#include "effect_13.h"

extern void DuelEffect_RunStatPenalty(void *work, s32 phase, s16 number) __asm__("func_801503F8");
extern void DuelEffect_InitJaggedColumnVertices(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices) __asm__("func_8014FABC");

extern DuelEffect13Config gDuelEffect_StatPenaltyConfigs[4] __asm__("D_8015AE94");
extern VECTOR gDuelEffect_StatPenaltyInitialScale __asm__("D_80146178");
extern GsIMAGE gDuelEffect_StatPenaltyTextureImages[21] __asm__("D_8015AC48");

extern void func_801503F8(void *work, s32 phase, s16 number);
extern void func_8014FABC(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices);

#endif

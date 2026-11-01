#ifndef MEMORIES_DECOMP_DUEL_ROW_STRIKE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_ROW_STRIKE_SEMANTIC_VIEWS_H

#include "effect_11.h"
#include "effect_16.h"

extern void DuelEffect_RunCardBreakup(void *work, s32 phase);
extern void DuelEffect_RunRowStrike(void *work, s32 phase);

extern DuelEffect11Config gDuelEffect_CardBreakupConfig;
extern DuelEffect16Config gDuelEffect_RowStrikeConfig;
extern VECTOR gDuelEffect_CardBreakupInitialScale;
extern VECTOR gDuelEffect_RowStrikeInitialScale;

#endif

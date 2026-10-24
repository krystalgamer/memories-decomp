#ifndef MEMORIES_DECOMP_DUEL_ROW_STRIKE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_ROW_STRIKE_SEMANTIC_VIEWS_H

#include "effect_11.h"
#include "effect_16.h"

extern void DuelEffect_RunCardBreakup(void *work, s32 phase) __asm__("func_80146760");
extern void DuelEffect_RunRowStrike(void *work, s32 phase) __asm__("func_80151558");

extern DuelEffect11Config gDuelEffect_CardBreakupConfig __asm__("D_8015A4E4");
extern DuelEffect16Config gDuelEffect_RowStrikeConfig __asm__("D_8015AEF4");
extern VECTOR gDuelEffect_CardBreakupInitialScale __asm__("D_80146004");
extern VECTOR gDuelEffect_RowStrikeInitialScale __asm__("D_80146198");

extern void func_80146760(void *work, s32 phase);
extern void func_80151558(void *work, s32 phase);

#endif

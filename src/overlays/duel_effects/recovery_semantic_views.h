#ifndef MEMORIES_DECOMP_DUEL_RECOVERY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_RECOVERY_SEMANTIC_VIEWS_H

#include "effect_5.h"
#include "effect_9.h"

extern void DuelEffect_RunLifePointRecovery(void *work, s32 phase) __asm__("func_80157E10");
extern void DuelEffect_RunInvertedLifePointRecovery(void *work, s32 phase) __asm__("func_801593A8");

extern DuelEffect5Config gDuelEffect_LifePointRecoveryConfigs[10] __asm__("D_8015B450");
extern DuelEffect9Config gDuelEffect_InvertedLifePointRecoveryConfigs[5] __asm__("D_8015B650");
extern VECTOR gDuelEffect_LifePointRecoveryInitialScale __asm__("D_80146218");
extern VECTOR gDuelEffect_InvertedLifePointRecoveryInitialScale __asm__("D_80146238");

extern void func_80157E10(void *work, s32 phase);
extern void func_801593A8(void *work, s32 phase);

#endif

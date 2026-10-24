#ifndef MEMORIES_DECOMP_DUEL_FIELD_BURST_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_FIELD_BURST_SEMANTIC_VIEWS_H

#include "effect_8.h"
#include "effect_12.h"

extern void DuelEffect_RunFieldColumnBurst(void *work, s32 phase) __asm__("func_80147B18");
extern void DuelEffect_RunStopDefenseBurst(void *work, s32 phase) __asm__("func_80150E00");
extern DuelEffect8Config gDuelEffect_FieldColumnBurstConfigs[6] __asm__("D_8015A514");
extern VECTOR gDuelEffect_FieldColumnBurstInitialScale __asm__("D_80146014");
extern DuelEffect12Config gDuelEffect_StopDefenseBurstConfig __asm__("D_8015AEE4");
extern VECTOR gDuelEffect_StopDefenseBurstInitialScale __asm__("D_80146188");

extern void func_80147B18(void *work, s32 phase);
extern void func_80150E00(void *work, s32 phase);

#endif

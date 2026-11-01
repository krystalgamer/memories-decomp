#ifndef MEMORIES_DECOMP_DUEL_FIELD_BURST_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_FIELD_BURST_SEMANTIC_VIEWS_H

#include "effect_8.h"
#include "effect_12.h"

extern void DuelEffect_RunFieldColumnBurst(void *work, s32 phase);
extern void DuelEffect_RunStopDefenseBurst(void *work, s32 phase);
extern DuelEffect8Config gDuelEffect_FieldColumnBurstConfigs[6];
extern VECTOR gDuelEffect_FieldColumnBurstInitialScale;
extern DuelEffect12Config gDuelEffect_StopDefenseBurstConfig;
extern VECTOR gDuelEffect_StopDefenseBurstInitialScale;

#endif

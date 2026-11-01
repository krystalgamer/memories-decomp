#ifndef MEMORIES_DECOMP_DUEL_DAMAGE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DAMAGE_SEMANTIC_VIEWS_H

#include "effect_6.h"
#include "effect_7.h"

extern void DuelEffect_RunDirectDamage(void *buffer, s32 phase);
extern void DuelEffect_RunReflectedDirectDamage(void *buffer, s32 phase);

extern DuelEffect6Config gDuelEffect_DirectDamageConfigs[6];
extern DuelEffect7Config gDuelEffect_ReflectedDirectDamageConfigs[5];
extern VECTOR gDuelEffect_DirectDamageInitialScale;
extern VECTOR gDuelEffect_ReflectedDirectDamageInitialScale;

#endif

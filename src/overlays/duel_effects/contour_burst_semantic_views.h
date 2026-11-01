#ifndef MEMORIES_DECOMP_DUEL_CONTOUR_BURST_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_CONTOUR_BURST_SEMANTIC_VIEWS_H

#include "effect_0.h"
#include "effect_18.h"

extern void DuelEffect_RunContourBurst(void *work, s32 phase);
extern void DuelEffect_RunParticleContourBurst(void *work, s32 phase);

extern DuelEffect0Config gDuelEffect_ContourBurstConfigs[30];
extern DuelEffect18Config gDuelEffect_ParticleContourBurstConfigs[2];
extern VECTOR gDuelEffect_ContourBurstInitialScale;
extern VECTOR gDuelEffect_ParticleContourBurstInitialScale;

#endif

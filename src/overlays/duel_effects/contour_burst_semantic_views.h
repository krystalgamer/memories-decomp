#ifndef MEMORIES_DECOMP_DUEL_CONTOUR_BURST_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_CONTOUR_BURST_SEMANTIC_VIEWS_H

#include "effect_0.h"
#include "effect_18.h"

extern void DuelEffect_RunContourBurst(void *work, s32 phase) __asm__("func_80154688");
extern void DuelEffect_RunParticleContourBurst(void *work, s32 phase) __asm__("func_80154084");

extern DuelEffect0Config gDuelEffect_ContourBurstConfigs[30] __asm__("D_8015B0B4");
extern DuelEffect18Config gDuelEffect_ParticleContourBurstConfigs[2] __asm__("D_8015B078");
extern VECTOR gDuelEffect_ContourBurstInitialScale __asm__("D_801461E8");
extern VECTOR gDuelEffect_ParticleContourBurstInitialScale __asm__("D_801461D8");

extern void func_80154688(void *work, s32 phase);
extern void func_80154084(void *work, s32 phase);

#endif

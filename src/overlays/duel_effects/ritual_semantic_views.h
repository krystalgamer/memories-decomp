#ifndef MEMORIES_DECOMP_DUEL_RITUAL_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_RITUAL_SEMANTIC_VIEWS_H

#include "effect_22.h"

extern void DuelEffect_RunRitual(void *work, s32 phase) __asm__("func_8014A8E4");
extern DuelEffect22Config gDuelEffect_RitualConfigs[24] __asm__("D_8015A658");
extern VECTOR gDuelEffect_RitualInitialScale __asm__("D_80146044");

extern void func_8014A8E4(void *work, s32 phase);

#endif

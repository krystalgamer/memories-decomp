#ifndef MEMORIES_DECOMP_DUEL_CARD_FLASH_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_CARD_FLASH_SEMANTIC_VIEWS_H

#include "effect_4.h"
#include "effect_19.h"

extern void DuelEffect_RunCardFlashBurst(void *work, s32 phase) __asm__("func_80159AAC");
extern void DuelEffect_RunExpandingRingBurst(void *work, s32 phase) __asm__("func_80153ADC");
extern DuelEffect4Config gDuelEffect_CardFlashBurstConfigs[2] __asm__("D_8015B704");
extern VECTOR gDuelEffect_CardFlashBurstInitialScale __asm__("D_80146248");
extern VECTOR gDuelEffect_ExpandingRingBurstInitialScale __asm__("D_801461C8");

extern void func_80159AAC(void *work, s32 phase);
extern void func_80153ADC(void *work, s32 phase);

#endif

#ifndef MEMORIES_DECOMP_DUEL_SELECTED_CARDS_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_SELECTED_CARDS_SEMANTIC_VIEWS_H

#include "effect_15.h"

extern void DuelEffect_RunSelectedCardOverlay(void *work, s32 phase) __asm__("func_8014FF40");
extern void DuelEffect_RunBackRowSweep(void *work, s32 phase) __asm__("func_80152048");
extern DuelEffect15Config gDuelEffect_SelectedCardOverlayConfigs[8] __asm__("D_8015AC08");
extern VECTOR gDuelEffect_SelectedCardOverlayInitialScale __asm__("D_80146168");
extern VECTOR gDuelEffect_BackRowSweepInitialScale __asm__("D_801461A8");

extern void func_8014FF40(void *work, s32 phase);
extern void func_80152048(void *work, s32 phase);

#endif

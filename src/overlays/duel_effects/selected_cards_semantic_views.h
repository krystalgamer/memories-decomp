#ifndef MEMORIES_DECOMP_DUEL_SELECTED_CARDS_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_SELECTED_CARDS_SEMANTIC_VIEWS_H

#include "effect_15.h"

extern void DuelEffect_RunSelectedCardOverlay(void *work, s32 phase);
extern void DuelEffect_RunBackRowSweep(void *work, s32 phase);
extern DuelEffect15Config gDuelEffect_SelectedCardOverlayConfigs[8];
extern VECTOR gDuelEffect_SelectedCardOverlayInitialScale;
extern VECTOR gDuelEffect_BackRowSweepInitialScale;

#endif

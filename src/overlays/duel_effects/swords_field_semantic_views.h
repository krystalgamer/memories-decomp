#ifndef MEMORIES_DECOMP_DUEL_SWORDS_FIELD_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_SWORDS_FIELD_SEMANTIC_VIEWS_H

#include "effect_21.h"

extern void DuelEffect_RunSwordsField(void *work, s32 phase) __asm__("func_8014E3EC");
extern SVECTOR gDuelEffect_SwordsFieldTilePositions[10][2] __asm__("D_8015AB68");
extern VECTOR gDuelEffect_SwordsFieldInitialScale __asm__("D_80146158");

extern void func_8014E3EC(void *work, s32 phase);

#endif

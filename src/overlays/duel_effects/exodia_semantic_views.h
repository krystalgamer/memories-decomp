#ifndef MEMORIES_DECOMP_DUEL_EXODIA_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_EXODIA_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_RunExodiaBurst(void *work, s32 phase) __asm__("func_8014D3E8");
extern VECTOR gDuelEffect_ExodiaInitialScale __asm__("D_80146148");
extern u32 gDuelEffect_CardObjectScratchWords[21] __asm__("D_8015B7A0");

extern void func_8014D3E8(void *work, s32 phase);

#endif

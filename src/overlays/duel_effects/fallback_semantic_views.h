#ifndef MEMORIES_DECOMP_DUEL_FALLBACK_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_FALLBACK_SEMANTIC_VIEWS_H

#include "../../types.h"

extern void DuelEffect_DrawErrorCross(s32 mode) __asm__("func_8014E35C");
extern char gDuelEffect_ErrorMessage[] __asm__("gNorthAmerican_D_80146014");
extern char gDuelEffect_InvalidVariantMessage[] __asm__("gNorthAmerican_D_8014602C");

extern void func_8014E35C(s32 mode);

#endif

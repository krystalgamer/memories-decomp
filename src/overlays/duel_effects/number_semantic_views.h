#ifndef MEMORIES_DECOMP_DUEL_NUMBER_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_NUMBER_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern s32 DuelEffect_PowS16(s16 base, u16 exponent);
extern u16 DuelEffect_CountDecimalDigitsS16(s16 value);
extern void DuelEffect_WriteNumberDigits(u16 value, u16 *digits);
extern void DuelEffect_DrawProjectedNumber(s32 value, u8 *color, SVECTOR *offset, u16 mode, u16 size, u16 bias);

#endif

#ifndef MEMORIES_DECOMP_DUEL_NUMBER_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_NUMBER_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

/* Keep mixed-width numeric contracts and native renderer ownership intact. */
extern s32 DuelEffect_PowS16(s16 base, u16 exponent) __asm__("func_8014F524");
extern u16 DuelEffect_CountDecimalDigitsS16(s16 value) __asm__("func_80156AD4");
extern void DuelEffect_WriteNumberDigits(u16 value, u16 *digits) __asm__("func_80156B40");
extern void DuelEffect_DrawProjectedNumber(s32 value, u8 *color, SVECTOR *offset, u16 mode, u16 size, u16 bias) __asm__("func_801566D4");

extern s32 func_8014F524(s16 base, u16 exponent);
extern u16 func_80156AD4(s16 value);
extern void func_80156B40(u16 value, u16 *digits);
extern void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode, u16 size, u16 bias);

#endif

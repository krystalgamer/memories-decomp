#ifndef MEMORIES_DECOMP_DUEL_PRIMITIVE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_PRIMITIVE_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawSharedBaseTriangles(u8 *color, u16 width, SVECTOR *positions, u16 count, s16 bias) __asm__("func_80156E58");
extern void DuelEffect_DrawProjectedFlatQuad(u8 *color, SVECTOR *vertices, u16 flags, u16 mode) __asm__("func_80156FA4");
extern u16 DuelEffect_DrawGradientPath(u8 *color, u16 count, SVECTOR *vertices, u16 bias, u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset) __asm__("func_801570B0");

extern void func_80156E58(u8 *color, u16 width, SVECTOR *positions, u16 count, s16 bias);
extern void func_80156FA4(u8 *color, SVECTOR *vertices, u16 flags, u16 mode);
extern u16 func_801570B0(u8 *color, u16 count, SVECTOR *vertices, u16 bias, u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset);

#endif

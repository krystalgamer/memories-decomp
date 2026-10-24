#ifndef MEMORIES_DECOMP_DUEL_DRAWING_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DRAWING_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawQuadContour(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags) __asm__("func_801558F4");
extern void DuelEffect_DrawFixedTileQuad(u8 *color) __asm__("func_80155F94");
extern void DuelEffect_DrawRandomTileQuad(u8 *color, SVECTOR *vertices, s16 depth) __asm__("func_80156064");
extern void DuelEffect_DrawTexturedRingStrip(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode) __asm__("func_8015616C");
extern void DuelEffect_DrawGradientRingStrip(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias) __asm__("func_80156448");

extern void func_801558F4(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags);
extern void func_80155F94(u8 *color);
extern void func_80156064(u8 *color, SVECTOR *vertices, s16 depth);
extern void func_8015616C(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode);
extern void func_80156448(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias);

#endif

#ifndef MEMORIES_DECOMP_DUEL_DISPLAY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DISPLAY_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawQuadrantTileQuads(u8 *color, u16 size, s16 depth) __asm__("func_801573A8");
extern void DuelEffect_DrawTranslatedTileRect(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile) __asm__("func_80157494");
extern void DuelEffect_DrawGradientConnectionBands(u8 *color, DVECTOR *first, DVECTOR *last, u16 width) __asm__("func_801575CC");

extern void func_801573A8(u8 *color, u16 size, s16 depth);
extern void func_80157494(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile);
extern void func_801575CC(u8 *color, DVECTOR *first, DVECTOR *last, u16 width);

#endif

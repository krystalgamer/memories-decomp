#ifndef MEMORIES_DECOMP_DUEL_DISPLAY_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DISPLAY_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawQuadrantTileQuads(u8 *color, u16 size, s16 depth);
extern void DuelEffect_DrawTranslatedTileRect(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile);
extern void DuelEffect_DrawGradientConnectionBands(u8 *color, DVECTOR *first, DVECTOR *last, u16 width);

#endif

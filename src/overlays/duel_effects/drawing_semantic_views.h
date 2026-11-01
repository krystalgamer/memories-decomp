#ifndef MEMORIES_DECOMP_DUEL_DRAWING_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DRAWING_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawQuadContour(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags);
extern void DuelEffect_DrawFixedTileQuad(u8 *color);
extern void DuelEffect_DrawRandomTileQuad(u8 *color, SVECTOR *vertices, s16 depth);
extern void DuelEffect_DrawTexturedRingStrip(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode);
extern void DuelEffect_DrawGradientRingStrip(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias);

#endif

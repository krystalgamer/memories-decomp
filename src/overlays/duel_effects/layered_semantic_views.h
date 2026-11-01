#ifndef MEMORIES_DECOMP_DUEL_LAYERED_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_LAYERED_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern void DuelEffect_DrawLayeredTileQuad(u8 *color, u16 size, s16 depth, SVECTOR *offset);
extern void DuelEffect_DrawLayeredGradientTrapezoid(u8 *color, u16 *widths, u16 height, s16 depth);

#endif

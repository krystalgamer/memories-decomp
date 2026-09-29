#ifndef MEMORIES_DECOMP_DUEL_LAYERED_DRAWING_H
#define MEMORIES_DECOMP_DUEL_LAYERED_DRAWING_H

#include "../../types.h"
#include "drawing_helpers.h"
#include "packet_helpers.h"
#include "textured_quads.h"

typedef struct {
    POLY_GT4 polygon;
    u8 alignment_gap[4];
    SVECTOR vertices[4];
} DuelEffectGradientQuad;

void func_801558F4(u8 *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags);
void func_80155BC0(u8 *color, u16 size, s16 depth, SVECTOR *offset);
void func_80155D90(u8 *color, u16 *widths, u16 height, s16 depth);

#endif

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

#endif

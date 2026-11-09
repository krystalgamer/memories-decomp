#ifndef FRENCH428_QUADS_VIEW_H
#define FRENCH428_QUADS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantQuadGroup groups[1];
    u8 unknown_02FC[0x1FA0];
    POLY_FT4 polygon;
    u8 unknown_22C4[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2324[0x410];
    ModelVariantClock clock;
    ModelVariantQuadTiming *timing;
    u8 unknown_2754[0x24];
    s32 phase;
} Quads428State;

#endif

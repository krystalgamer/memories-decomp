#ifndef FRENCH429_QUADS_VIEW_H
#define FRENCH429_QUADS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantQuadGroup groups[1];
    u8 unknown_02FC[0x19F0];
    POLY_FT4 polygon;
    u8 unknown_1D14[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_1D74[0xD0];
    ModelVariantClock clock;
    ModelVariantQuadTiming *timing;
    u8 unknown_1E64[0x24];
    s32 phase;
} Quads429State;

#endif

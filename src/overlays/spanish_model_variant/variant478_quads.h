#ifndef SPANISH_MODEL_VARIANT478_QUADS_H
#define SPANISH_MODEL_VARIANT478_QUADS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantQuadGroup groups[1];
    u8 unknown_02FC[0x264C];
    POLY_FT4 polygon;
    u8 unknown_2970[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_29D0[0x1C];
    u32 time;
    u8 unknown_29F0[4];
    s32 step;
    u8 unknown_29F8[4];
    ModelVariantQuadTiming *timing;
    u8 unknown_2A00[0x24];
    s32 phase;
} Quads478State;

#endif

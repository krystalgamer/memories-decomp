#ifndef SPANISH_MODEL_VARIANT470_QUADS_H
#define SPANISH_MODEL_VARIANT470_QUADS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0x988];
    ModelVariantQuadGroup groups[1];
    u8 unknown_0C84[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2538[0x6C];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_25C0[0x1C];
    u32 time;
    u8 unknown_25E0[4];
    s32 step;
    u8 unknown_25E8[4];
    ModelVariantQuadTiming *timing;
    u8 unknown_25F0[0x30];
    s32 phase;
} Quads470State;

#endif

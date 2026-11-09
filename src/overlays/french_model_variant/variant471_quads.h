#ifndef FRENCH471_QUADS_VIEW_H
#define FRENCH471_QUADS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0x794];
    ModelVariantQuadGroup groups[1];
    u8 unknown_0A90[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2344[0x6C];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_23CC[0x10];
    ModelVariantClock clock;
    ModelVariantQuadTiming *timing;
    u8 unknown_23FC[0x30];
    s32 phase;
} Quads471State;

#endif

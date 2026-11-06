#ifndef FRENCH419_QUADS_VIEW_H
#define FRENCH419_QUADS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    ModelVariantQuadGroupView groups[1];
    u8 unknown_02FC[0x1F20];
    POLY_FT4 polygon;
    u8 unknown_2244[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_22A4[0x1C];
    u32 time;
    u8 unknown_22C4[4];
    s32 step;
    u8 unknown_22CC[4];
    ModelVariantQuadTimingLong *timing;
    u8 unknown_22D4[0x20];
    s32 phase;
} Quads419State;

#endif

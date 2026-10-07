#ifndef SPANISH_MODEL_VARIANT473_FUNNELS_H
#define SPANISH_MODEL_VARIANT473_FUNNELS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} Funnel473;

typedef struct {
    u8 unknown_000[0x4EC];
    s32 size;
    u8 unknown_4F0[8];
} Funnel473Object;

typedef struct {
    Funnel473Object objects[5];
    u8 unknown_18D8[0x10AC];
    ModelVariantSheet sheets[2];
    u8 unknown_2AB4[0x8C8];
    Funnel473 funnels[5];
    u8 unknown_38F4[0x34];
    POLY_GT4 quad;
    u8 unknown_395C[0x1B4];
    VECTOR positions[5];
    s32 direction[3];
    u8 unknown_3B6C[0x70];
    s32 frame;
    u8 unknown_3BE0[8];
    s32 step;
    u8 unknown_3BEC[0x1C];
    s32 phase;
    u8 unknown_3C0C[0x34];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 spin;
} Funnel473State;

#endif

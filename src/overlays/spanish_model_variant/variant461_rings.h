#ifndef SPANISH_MODEL_VARIANT461_RINGS_H
#define SPANISH_MODEL_VARIANT461_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0xB90];
    ModelVariantQuadRing ring;
    u8 unknown_1144[0x1D74];
    POLY_FT4 polygon;
    u8 unknown_2EE0[0x4C];
    s32 origin[3];
    u8 unknown_2F38[0xF8];
    s32 probe[2];
    u8 unknown_3038[4];
    s16 delta_x;
    s16 delta_y;
    u8 unknown_3040[0x324];
    s32 step;
    u8 unknown_3368[0x18];
    s32 angle;
    s32 scale;
    s32 wave;
    u8 unknown_338C[0x20];
    s32 phase;
} Rings461State;

#endif

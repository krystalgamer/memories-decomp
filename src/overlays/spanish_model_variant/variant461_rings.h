#ifndef SPANISH_MODEL_VARIANT461_RINGS_H
#define SPANISH_MODEL_VARIANT461_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[24];
    SVECTOR v1[24];
    SVECTOR v2[24];
    SVECTOR v3[24];
    u8 unknown_300[0xD4];
    s32 size[24];
} Ring461;

typedef struct {
    u8 unknown_0000[0xB90];
    Ring461 ring;
    u8 unknown_0FC4[0x1EF4];
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

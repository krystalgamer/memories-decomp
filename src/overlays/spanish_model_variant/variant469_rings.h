#ifndef SPANISH_MODEL_VARIANT469_RINGS_H
#define SPANISH_MODEL_VARIANT469_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[24];
    SVECTOR v1[24];
    SVECTOR v2[24];
    SVECTOR v3[24];
    u8 unknown_300[0xD4];
    s32 size[24];
} Ring469;

typedef struct {
    u8 unknown_0000[0x1614];
    Ring469 ring;
    u8 unknown_1A48[0x1A2C];
    POLY_FT4 polygon;
    u8 unknown_349C[0x44];
    s32 origin[3];
    u8 unknown_34EC[0x160];
    s32 probe[2];
    u8 unknown_3654[0x14];
    s16 delta_x;
    s16 delta_y;
    u8 unknown_366C[0x324];
    s32 step;
    u8 unknown_3994[0x18];
    s32 angle;
    s32 scale;
    s32 wave;
    u8 unknown_39B8[0x20];
    s32 phase;
} Rings469State;

#endif

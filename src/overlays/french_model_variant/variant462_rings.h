#ifndef FRENCH_MODEL_VARIANT462_RINGS_H
#define FRENCH_MODEL_VARIANT462_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[24];
    SVECTOR v1[24];
    SVECTOR v2[24];
    SVECTOR v3[24];
    u8 unknown_300[0xD4];
    s32 size[24];
} Ring462;

typedef struct {
    u8 unknown_0000[0xD70];
    Ring462 ring;
    u8 unknown_11A4[0x1EF4];
    POLY_FT4 polygon;
    u8 unknown_30C0[0x4C];
    s32 origin[3];
    u8 unknown_3118[0xF8];
    s32 probe[2];
    u8 unknown_3218[4];
    s16 delta_x;
    s16 delta_y;
    u8 unknown_3220[0x324];
    s32 step;
    u8 unknown_3548[0x18];
    s32 angle;
    s32 scale;
    s32 wave;
    u8 unknown_356C[0x20];
    s32 phase;
} Rings462State;

#endif

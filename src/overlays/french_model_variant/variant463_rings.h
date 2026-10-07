#ifndef FRENCH_MODEL_VARIANT463_RINGS_H
#define FRENCH_MODEL_VARIANT463_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[24];
    SVECTOR v1[24];
    SVECTOR v2[24];
    SVECTOR v3[24];
    u8 unknown_300[0xD4];
    s32 size[24];
} Ring463;

typedef struct {
    u8 unknown_0000[0x2710];
    Ring463 ring;
    u8 unknown_2B44[0x18DC];
    POLY_FT4 polygon;
    u8 unknown_4448[0x4C];
    s32 origin[3];
    u8 unknown_44A0[0x288];
    s32 probe[2];
    u8 unknown_4730[4];
    s16 delta_x;
    s16 delta_y;
    u8 unknown_4738[0x324];
    s32 step;
    u8 unknown_4A60[0x20];
    s32 angle;
    s32 scale;
    s32 wave;
    u8 unknown_4A8C[0x24];
    s32 phase;
} Rings463State;

#endif

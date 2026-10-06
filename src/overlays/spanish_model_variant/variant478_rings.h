#ifndef SPANISH_MODEL_VARIANT478_RINGS_H
#define SPANISH_MODEL_VARIANT478_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR v0[8];
    SVECTOR v1[8];
    SVECTOR v2[8];
    SVECTOR v3[8];
    u8 unknown_100[0x40];
    u8 color[4];
    s32 size[8];
    u8 unknown_164[0x20];
    s32 hidden[8];
    u8 unknown_1A4[0xC0];
} Ring478;

typedef struct {
    u8 unknown_00[0x28];
    u32 grow_end;
    u32 fade_begin;
} Rings478Timing;

typedef struct {
    u8 unknown_0000[0x2FC];
    Ring478 rings[3];
    u8 unknown_0A28[0x1F20];
    POLY_FT4 polygon;
    u8 unknown_2970[0x38];
    VECTOR origin;
    u8 unknown_29B8[8];
    s32 probe_words[2];
    u8 unknown_29C8[4];
    SVECTOR projected_delta;
    u8 unknown_29D4[0x18];
    u32 time;
    u8 unknown_29F0[4];
    u32 step;
    u8 unknown_29F8[4];
    Rings478Timing *G32 timing;
    u8 unknown_2A00[0x1C];
    s32 wave;
} Rings478State;

#endif

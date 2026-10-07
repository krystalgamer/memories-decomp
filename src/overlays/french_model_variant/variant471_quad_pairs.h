#ifndef FRENCH_MODEL_VARIANT471_QUAD_PAIRS_H
#define FRENCH_MODEL_VARIANT471_QUAD_PAIRS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    VECTOR position;
    u8 unknown_10[0x10];
} QuadPairAnchor471;

typedef struct {
    SVECTOR a[16];
    SVECTOR b[16];
    SVECTOR c[16];
    SVECTOR d[16];
    u8 unknown_200[0x80];
    u8 color[4];
    u8 unknown_284[0x10];
    s32 size[16];
    s32 done[16];
    u8 unknown_314[0x40];
    s32 reset[16];
    u8 unknown_394[0x14];
    QuadPairAnchor471 anchor[16];
    u8 unknown_5A8[0xEC];
    VECTOR direction[16];
} QuadPairGroup471;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} QuadPairTiming471;

typedef struct {
    QuadPairGroup471 groups[1];
    u8 unknown_0794[0x1BB0];
    POLY_FT4 polygon;
    u8 unknown_236C[0x4C];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_23CC[0x1C];
    u32 time;
    u8 unknown_23EC[4];
    s32 step;
    u8 unknown_23F4[4];
    QuadPairTiming471 *G32 timing;
    u8 unknown_23FC[0x30];
    s32 phase;
} QuadPairState471;

#endif

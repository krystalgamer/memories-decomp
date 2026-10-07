#ifndef FRENCH_MODEL_VARIANT470_QUAD_PAIRS_H
#define FRENCH_MODEL_VARIANT470_QUAD_PAIRS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    VECTOR position;
    u8 unknown_10[0x10];
} QuadPairAnchor470;

/* One 0x4C4-byte group: ten quads, their growth, done and reset flags,
 * anchors and drift directions. */
typedef struct {
    SVECTOR a[10];
    SVECTOR b[10];
    SVECTOR c[10];
    SVECTOR d[10];
    u8 unknown_140[0x50];
    u8 color[4];
    u8 unknown_194[0x10];
    s32 size[10];
    s32 done[10];
    u8 unknown_1F4[0x28];
    s32 reset[10];
    u8 unknown_244[0x14];
    QuadPairAnchor470 anchor[10];
    u8 unknown_398[0x8C];
    VECTOR direction[10];
} QuadPairGroup470;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} QuadPairTiming470;

typedef struct {
    QuadPairGroup470 groups[2];
    u8 unknown_0988[0x1BB0];
    POLY_FT4 polygon;
    u8 unknown_2560[0x4C];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_25C0[0x1C];
    u32 time;
    u8 unknown_25E0[4];
    s32 step;
    u8 unknown_25E8[4];
    QuadPairTiming470 *G32 timing;
    u8 unknown_25F0[0x30];
    s32 phase;
} QuadPairState470;

#endif

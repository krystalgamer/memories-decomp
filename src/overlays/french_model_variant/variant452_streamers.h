#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT452_STREAMERS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT452_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown00[0x28];
    SVECTOR a[5];
    PSXLONG sa[5];
    s32 angle[5];
    SVECTOR b[5];
    PSXLONG sb[5];
    s32 width[5];
    CVECTOR inner[5];
    CVECTOR outer[5];
    u8 unknownF0[4];
    s32 depth[5];
    s16 ox[5];
    s16 oy[5];
    u8 unknown11C[0x14];
} Variant452Streamer;

typedef struct {
    u8 unknown0000[0x720];
    Variant452Streamer streamers[16];
    u8 unknown1A20[0x10A8];
    ModelVariantSheet sheet;
    u8 unknown2B60[0x670];
    POLY_GT4 poly;
    u8 unknown3204[0x104];
    s32 translation[3];
    u8 unknown3314[0x28];
    VECTOR delta;
    u8 unknown334C[8];
    s32 flags;
    u8 unknown3358[8];
    s32 step;
    u8 unknown3364[4];
    s16 rotation;
    s16 field336A;
    s16 field336C;
    u8 unknown336E[0x2A];
    s32 radius;
    u8 unknown339C[0xC];
    s32 phase;
} Variant452StreamersView;

#endif

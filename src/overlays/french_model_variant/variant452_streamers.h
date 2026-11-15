#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT452_STREAMERS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT452_STREAMERS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown00[0x28];
    SVECTOR a[5];
    long sa[5];
    s32 angle[5];
    SVECTOR b[5];
    long sb[5];
    s32 width[5];
    CVECTOR inner[5];
    CVECTOR outer[5];
    u8 unknownF0[4];
    s32 depth[5];
    s16 ox[5];
    s16 oy[5];
    u8 unknown11C[0x14];
} Variant452Streamer;

#endif

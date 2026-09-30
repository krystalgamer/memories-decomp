#ifndef MEMORIES_DECOMP_MODEL_VARIANT458_QUADS_H
#define MEMORIES_DECOMP_MODEL_VARIANT458_QUADS_H

#include "model_variant.h"

/* One 0x1E4-byte set of eight quads; only the fields used here are named. */
typedef struct {
    SVECTOR a[8];
    SVECTOR b[8];
    SVECTOR c[8];
    SVECTOR d[8];
    u8 unknown100[0x40];
    CVECTOR color;
    u8 unknown144[0x10];
    s32 level[8];
    s32 hidden[8];
    u8 unknown194[0x50];
} Variant458Quads;

#endif

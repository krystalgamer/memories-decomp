#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT562_SHARDS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT562_SHARDS_H

#include "../model_variant/model_variant.h"

/* One 0x74-byte shard of header 562: two triangles' worth of points, their
 * projected screen coordinates, a growth state, the two growth scales and the
 * two projected depths. */
typedef struct {
    SVECTOR v[3][2];
    PSXLONG sxy[3][2];
    u8 unknown48[0x1A];
    s16 state;
    s32 scale[2];
    s32 otz[2];
} Variant562Shard;

/* The shard count lives at +0xC of the record the work area points to. */
typedef struct {
    u8 unknown0[0xC];
    s32 count;
} Variant562ShardInfo;

#endif

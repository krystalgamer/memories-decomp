#ifndef MEMORIES_DECOMP_MODEL_VARIANT414_FAN_H
#define MEMORIES_DECOMP_MODEL_VARIANT414_FAN_H

#include "model_variant.h"

/* One 0x74-byte fan: a center, ten edge points, two colors, scale, and done. */
typedef struct {
    SVECTOR point[11];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    u8 unknown64[12];
    s32 done;
} Variant414Fan;

#endif

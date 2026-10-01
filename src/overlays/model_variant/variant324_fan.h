#ifndef MEMORIES_DECOMP_MODEL_VARIANT324_FAN_H
#define MEMORIES_DECOMP_MODEL_VARIANT324_FAN_H

#include "model_variant.h"

/* One 0x70-byte fan: a center, ten edge points, two colors, and scale. */
typedef struct {
    SVECTOR point[11];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    u8 unknown64[12];
} Variant324Fan;

#endif

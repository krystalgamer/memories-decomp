#ifndef MEMORIES_DECOMP_MODEL_VARIANT459_CURTAINS_H
#define MEMORIES_DECOMP_MODEL_VARIANT459_CURTAINS_H

#include "model_variant.h"

/* One 0x1AC-byte curtain: two 17-point rows, rotation, and scale. */
typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    u8 unknown110[0x88];
    SVECTOR rotation;
    s32 scale;
    u8 unknown1a4[8];
} Variant459Curtain;

#endif

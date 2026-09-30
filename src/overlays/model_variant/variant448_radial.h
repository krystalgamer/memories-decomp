#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_RADIAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_RADIAL_H

#include "model_variant.h"

/* One 0x128-byte radial record: center and ring points, three colors, size. */
typedef struct {
    SVECTOR point[35];
    CVECTOR center;
    CVECTOR middle;
    CVECTOR edge;
    s32 size;
} Variant448Radial;

#endif

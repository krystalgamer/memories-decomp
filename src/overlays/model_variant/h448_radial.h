#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_RADIAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_RADIAL_H

#include "model_variant.h"

/* One 0x128-byte radial record: the centre point and seventeen inner-ring
 * points, seventeen outer-ring points, three colours and a size. */
typedef struct {
    SVECTOR inner[18];
    SVECTOR outer[17];
    CVECTOR center;
    CVECTOR middle;
    CVECTOR edge;
    s32 size;
} Variant448Radial;

#endif

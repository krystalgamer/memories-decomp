#ifndef MEMORIES_DECOMP_MODEL_VARIANT405_VEILS_H
#define MEMORIES_DECOMP_MODEL_VARIANT405_VEILS_H

#include "model_variant.h"

/* One 0x1A0-byte veil of header 405: three rows of seventeen points, a scale
 * and a wrap count. */
typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    SVECTOR c[17];
    s32 scale;
    s32 count;
} Variant405Veil;

#endif

#ifndef SPANISH_MODEL_VARIANT439_SCREEN_RINGS_H
#define SPANISH_MODEL_VARIANT439_SCREEN_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One of the three 0x1A8-byte screen-textured rings at work + 0xE08: three
 * rows of seventeen points, inner and outer colours, a scale and a wrap
 * count. */
typedef struct {
    SVECTOR a[17], b[17], c[17];
    CVECTOR inner, outer;
    s32 scale, count;
} Variant439ScreenRing;

#endif

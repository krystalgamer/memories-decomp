#ifndef VARIANT335_RING_VIEW_H
#define VARIANT335_RING_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    SVECTOR c[17];
    CVECTOR inner;
    CVECTOR outer;
    s32 scale;
    s32 cycles;
} Variant335Ring;

#endif

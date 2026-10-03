#ifndef VARIANT335_RING_VIEW_H
#define VARIANT335_RING_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    SVECTOR c[17];
    u8 inner[4];
    u8 outer[4];
    s32 scale;
    s32 cycles;
} Variant335Ring;

#endif

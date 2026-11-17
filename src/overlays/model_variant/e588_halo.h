#ifndef MEMORIES_DECOMP_MODEL_VARIANT405_HALO_H
#define MEMORIES_DECOMP_MODEL_VARIANT405_HALO_H

#include "model_variant.h"

/* The header-405 halo block at work + 0x820: five rings of seventeen points
 * in a top and a bottom row, and per ring the two row heights and a wrap
 * count. */
typedef struct {
    SVECTOR top[5][17];
    SVECTOR bottom[5][17];
    u8 unknown550[0x28];
    s32 rise[5];
    s32 fall[5];
    s32 count[5];
} Variant405Halo;

#endif

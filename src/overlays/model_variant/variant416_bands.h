#ifndef MEMORIES_DECOMP_MODEL_VARIANT416_BANDS_H
#define MEMORIES_DECOMP_MODEL_VARIANT416_BANDS_H

#include "model_variant.h"

/* One 0xA8-byte wing of header 416: two points per row, their screen
 * coordinates, the growth level and done flag per point, two colour rows and
 * the two depths. */
typedef struct {
    SVECTOR a[2];
    SVECTOR b[2];
    SVECTOR c[2];
    PSXLONG sa[2];
    PSXLONG sb[2];
    PSXLONG sc[2];
    u8 pad48[0x20];
    s32 level[2];
    s32 done[2];
    u8 ca[2][4];
    u8 cb[2][4];
    u8 pad88[0x18];
    s32 otz[2];
} Variant416Band;

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT423_BANDS_H
#define MEMORIES_DECOMP_MODEL_VARIANT423_BANDS_H

#include "model_variant.h"

/* One 0x108-byte band of header 423: three rows of five points, their screen
 * coordinates, two colour rows and the five depths. */
typedef struct {
    SVECTOR a[5];
    SVECTOR b[5];
    SVECTOR c[5];
    PSXLONG sa[5];
    PSXLONG sb[5];
    PSXLONG sc[5];
    u8 ca[5][4];
    u8 cb[5][4];
    u8 padDC[0x18];
    s32 otz[5];
} Variant423Band;

#endif

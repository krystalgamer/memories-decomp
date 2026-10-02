#ifndef MEMORIES_DECOMP_MODEL_VARIANT324_BANDS_H
#define MEMORIES_DECOMP_MODEL_VARIANT324_BANDS_H

#include "model_variant.h"

/* Header 324's 0x1EC-byte band record: three rows of nine points, their
 * screen coordinates, two colour rows, and per point the depth and the flag
 * RotTransPers3 writes. */
typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    PSXLONG sa[9];
    PSXLONG sb[9];
    PSXLONG sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
    PSXLONG flag[9];
} Variant324Band;

#endif

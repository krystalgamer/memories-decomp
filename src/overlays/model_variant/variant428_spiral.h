#ifndef MEMORIES_DECOMP_MODEL_VARIANT428_SPIRAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT428_SPIRAL_H

#include "model_variant.h"

/* One 0x7C-byte arm of header 428's spiral: a two-point spine and a copy of
 * it moved along the view, both projected, per point the screen angle, the
 * projected width and its screen offset, and two colour rows. */
typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 cb[2][4];
    u8 ca[2][4];
    u8 unknown60[4];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} Variant428SpiralArm;

#endif

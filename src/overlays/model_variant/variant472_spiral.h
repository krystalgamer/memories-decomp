#ifndef MEMORIES_DECOMP_MODEL_VARIANT472_SPIRAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT472_SPIRAL_H

#include "model_variant.h"

/* One 0x104-byte arm of header 472's spiral (twelve at work + 0x1EF4): a
 * four-point spine and a copy of it moved along the view, both projected, per
 * point the screen angle, the projected width, the outer and inner colours,
 * the projection flag, the depth and the width's screen offset. */
typedef struct {
    u8 unknown00[0x20];
    SVECTOR a[4];
    PSXLONG sa[4];
    s32 angle[4];
    SVECTOR b[4];
    PSXLONG sb[4];
    s32 width[4];
    u8 cb[4][4];
    u8 ca[4][4];
    u8 unknownC0[4];
    PSXLONG flag[4];
    s32 otz[4];
    s16 ox[4];
    s16 oy[4];
    u8 unknownF4[0x10];
} Variant472SpiralArm;

#endif

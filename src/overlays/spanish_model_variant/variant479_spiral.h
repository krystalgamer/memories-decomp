#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_SPIRAL_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_SPIRAL_H

#include "../model_variant/model_variant.h"

/* One 0xC4-byte arm of header 479's spiral: a three-point spine and a copy of
 * it moved along the view, both projected, per point the screen angle, the
 * projected width and its screen offset, and two colour rows. */

typedef struct {
    u8 unknown00[0x18];
    SVECTOR a[3];
    PSXLONG sa[3];
    s32 angle[3];
    SVECTOR b[3];
    PSXLONG sb[3];
    s32 width[3];
    u8 cb[3][4];
    u8 ca[3][4];
    u8 unknown90[4];
    PSXLONG flag[3];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    u8 unknownB8[0xC];
} Variant479SpiralArm;

#endif

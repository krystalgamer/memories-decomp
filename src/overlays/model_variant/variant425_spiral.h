#ifndef MEMORIES_DECOMP_MODEL_VARIANT425_SPIRAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT425_SPIRAL_H

#include "model_variant.h"

/* One 0x7C-byte arm of header 425's spiral (sixteen at work + 0x600): a
 * two-point spine and a copy of it moved along the view, both projected, per
 * point the screen angle, the projected width, the depth and the width's
 * screen offset. */
typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 unknown50[0x14];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} Variant425SpiralArm;

#endif

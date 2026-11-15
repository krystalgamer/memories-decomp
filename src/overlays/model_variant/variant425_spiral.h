#ifndef MEMORIES_DECOMP_MODEL_VARIANT425_SPIRAL_H
#define MEMORIES_DECOMP_MODEL_VARIANT425_SPIRAL_H

#include "model_variant.h"

/* One 0x7C-byte spiral arm as the Spanish spiral drawers view their records'
 * glow/spiral arrays: a two-point spine and a copy of it moved along the
 * view, both projected, per point the screen angle, the projected width, the
 * depth and the width's screen offset. It has the layout of
 * ModelVariantShadedRay, which header 425's own arms use directly. */
typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    u8 unknown50[0x14];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} Variant425SpiralArm;

#endif

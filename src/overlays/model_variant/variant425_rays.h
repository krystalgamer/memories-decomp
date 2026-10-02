#ifndef MEMORIES_DECOMP_MODEL_VARIANT425_RAYS_H
#define MEMORIES_DECOMP_MODEL_VARIANT425_RAYS_H

#include "model_variant.h"

/* One 0xB8-byte ray of header 425 (sixteen at work + 0xDC0): a three-point
 * spine and a copy of it moved along the view, both projected, per point the
 * screen angle, the projected width, the depth and the width's screen
 * offset. */
typedef struct {
    u8 unknown00[0x18];
    SVECTOR a[3];
    PSXLONG sa[3];
    s32 angle[3];
    SVECTOR b[3];
    PSXLONG sb[3];
    s32 width[3];
    u8 unknown78[0x1C];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    u8 unknownAC[0xC];
} Variant425Ray;

#endif

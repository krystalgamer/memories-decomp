#ifndef MEMORIES_DECOMP_MODEL_VARIANT443_RIBBONS_H
#define MEMORIES_DECOMP_MODEL_VARIANT443_RIBBONS_H

#include "model_variant.h"

/* One 0x2E8-byte ribbon of header 443: a seventeen-point spine and a copy of
 * it moved along the view, both projected, per point the screen angle and
 * projected width, a colour, the drawn length, the phase state, the view
 * offset, the depth and the width's screen offset. */
typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown224[0x1C];
    s32 count;
    u8 unknown244[4];
    s32 state;
    s32 len;
    u8 unknown250[0x10];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Variant443Ribbon;

#endif

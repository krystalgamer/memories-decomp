#ifndef MEMORIES_DECOMP_MODEL_VARIANT458_RIBBONS_H
#define MEMORIES_DECOMP_MODEL_VARIANT458_RIBBONS_H

#include "model_variant.h"

/* One 0x2E4-byte ribbon of header 458: a thirteen-point spine and a copy of
 * it moved along the view, both projected, per point the screen angle and
 * projected width, a colour, the drawn length, the phase state, the view
 * offset, the depth, the projection flag and the width's screen offset. */
typedef struct {
    SVECTOR a[13];
    PSXLONG sa[13];
    s32 angle[13];
    SVECTOR b[13];
    PSXLONG sb[13];
    s32 width[13];
    u8 color[4];
    u8 unknown1A4[0x1C];
    s32 count;
    u8 unknown1C4[4];
    s32 state;
    s32 len;
    u8 unknown1D0[0x78];
    s32 otz[13];
    PSXLONG flag[13];
    s16 ox[13];
    s16 oy[13];
} Variant458Ribbon;

#endif

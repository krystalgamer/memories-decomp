#ifndef MEMORIES_DECOMP_MODEL_VARIANT376_RIBBON_H
#define MEMORIES_DECOMP_MODEL_VARIANT376_RIBBON_H

#include "model_variant.h"

/* The 0x3A4-byte ribbon of header 376: a seventeen-point twisted spine and a
 * copy of it moved along the view, both projected, and per point the screen
 * angle, the projected width, the projection flag, the depth and the width's
 * screen offset. */
typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown224[0xB4];
    PSXLONG flag[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Variant376Ribbon;

#endif

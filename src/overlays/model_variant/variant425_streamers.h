#ifndef MEMORIES_DECOMP_MODEL_VARIANT425_STREAMERS_H
#define MEMORIES_DECOMP_MODEL_VARIANT425_STREAMERS_H

#include "model_variant.h"

/* One 0x64-byte two-point streamer of header 425 (at work + 0xF4C): the spine
 * and a copy of it moved along the view, both projected, per point the screen
 * angle, the projected width, the colour, the depth and the width's screen
 * offset. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 unknown40[0x08];
    u8 color[2][4];
    u8 unknown50[0x04];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
} Variant425Streamer;

#endif

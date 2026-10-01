#ifndef MEMORIES_DECOMP_MODEL_VARIANT458_STREAMERS_H
#define MEMORIES_DECOMP_MODEL_VARIANT458_STREAMERS_H

#include "model_variant.h"

/* One 0x334-byte streamer of header 458: a seventeen-point spine and a copy
 * of it moved along the view, both projected, and per point the screen angle,
 * the projected width, a colour, the depth and the width's screen offset. */
typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 unknown220[0x44];
    u8 color[17][4];
    u8 unknown2A8[0x04];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Variant458Streamer;

#endif

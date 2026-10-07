#ifndef MEMORIES_DECOMP_MODEL_VARIANT437_STREAMERS_H
#define MEMORIES_DECOMP_MODEL_VARIANT437_STREAMERS_H

#include "model_variant.h"

/* One 0x274-byte thirteen-point streamer of header 437 (three at work +
 * 0x93C): the spine and a copy of it moved along the view, both projected,
 * per point the screen angle, the projected width, the colour, the depth and
 * the width's screen offset. */
typedef struct {
    SVECTOR a[13];
    PSXLONG sa[13];
    s32 angle[13];
    SVECTOR b[13];
    PSXLONG sb[13];
    s32 width[13];
    u8 unknown1A0[0x34];
    u8 color[13][4];
    u8 unknown208[0x04];
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} Variant437Streamer;

#endif

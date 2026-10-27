#ifndef MEMORIES_DECOMP_FRENCH_MODEL_VARIANT469_RIBBONS_H
#define MEMORIES_DECOMP_FRENCH_MODEL_VARIANT469_RIBBONS_H

#include "../model_variant/model_variant.h"

/* One 0x250-byte ribbon of header 469: an eleven-point spine and a copy of
 * it moved along the view, both projected, per point the screen angle and
 * projected width, a colour, the drawn length, the phase state, the view
 * offset, the depth and the width's screen offset. */
typedef struct {
    SVECTOR a[11];
    PSXLONG sa[11];
    s32 angle[11];
    SVECTOR b[11];
    PSXLONG sb[11];
    s32 width[11];
    u8 color[4];
    u8 unknown164[0x1C];
    s32 count;
    u8 unknown184[4];
    s32 state;
    s32 len;
    u8 unknown190[0x68];
    s32 otz[11];
    s16 ox[11];
    s16 oy[11];
} Variant469Ribbon;

#endif

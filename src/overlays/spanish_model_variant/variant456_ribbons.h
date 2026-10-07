#ifndef SPANISH_MODEL_VARIANT456_RIBBONS_H
#define SPANISH_MODEL_VARIANT456_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0xE0-byte ribbon of header 456: a three-point spine and a copy of it
 * moved along the view, both projected, the screen angle, projected width and
 * screen offset per point, a colour, the depth per segment and a width scale. */
typedef struct {
    SVECTOR a[3];
    PSXLONG sa[3];
    s32 angle[3];
    SVECTOR b[3];
    PSXLONG sb[3];
    s32 width[3];
    u8 color[4];
    u8 unknown64[0x50];
    s32 otz[3];
    s16 ox[3];
    s16 oy[3];
    u8 unknownCC[0x10];
    s32 scale;
} Ribbon456;

#endif

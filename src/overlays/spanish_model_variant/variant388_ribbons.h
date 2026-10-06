#ifndef SPANISH_MODEL_VARIANT388_RIBBONS_H
#define SPANISH_MODEL_VARIANT388_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x74-byte ribbon of this family: the two-point spine, its sideways
 * copy, both projections, per point the screen angle, projected width and
 * offset, and the RotTransPers4 flag kept for the draw pass. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    PSXLONG flag[2];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} Ribbon388;

#endif

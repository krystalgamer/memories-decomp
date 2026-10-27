#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_RIBS_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT479_RIBS_H

#include "../model_variant/model_variant.h"

/* One 0x250-byte rib of header 479: a nine-point spine and a copy of it moved
 * along the view, both projected, per point the screen angle, the projected
 * width and its screen offset, two colour rows, and the size/fade/state
 * triple driving its growth. */

typedef struct {
    SVECTOR a[9];
    PSXLONG sa[9];
    s32 angle[9];
    SVECTOR b[9];
    PSXLONG sb[9];
    s32 width[9];
    u8 ca[9][4];
    u8 cb[9][4];
    s32 scale[3];
    u8 unknown174[4];
    s16 field_178, field_17A;
    s32 field_17C;
    s32 size;
    s32 fade;
    s32 state;
    u8 unknown18C[0x48];
    s32 direction[3];
    u8 unknown1E0[4];
    PSXLONG flag[9];
    s32 otz[9];
    s16 ox[9];
    s16 oy[9];
} Variant479Rib;

#endif

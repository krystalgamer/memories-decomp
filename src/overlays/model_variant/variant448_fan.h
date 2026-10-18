#ifndef MEMORIES_DECOMP_MODEL_VARIANT448_FAN_H
#define MEMORIES_DECOMP_MODEL_VARIANT448_FAN_H

#include "model_variant.h"

/* One 0x208-byte fan unit of header 448: two arms of three fans, each a
 * two-point spine and a copy of it moved along the view, both projected, per
 * point the screen angle, the projected width and its screen offset, then two
 * colour rows, the depths and the per-arm growth amounts and stages. */
typedef struct {
    SVECTOR a[2][3][2];
    PSXLONG sa[2][3][2];
    s32 angle[2][3][2];
    SVECTOR b[2][3][2];
    PSXLONG sb[2][3][2];
    s32 width[2][3][2];
    u8 cb[2][4];
    u8 ca[2][4];
    s32 otz[2][3][2];
    s16 ox[2][3][2];
    s16 oy[2][3][2];
    s32 amount[2][2];
    s32 stage[2];
} Variant448FanUnit;

#endif

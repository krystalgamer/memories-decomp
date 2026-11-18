#ifndef MEMORIES_DECOMP_MODEL_VARIANT411_BEAMS_H
#define MEMORIES_DECOMP_MODEL_VARIANT411_BEAMS_H

#include "model_variant.h"

/* One 0x64-byte two-point beam of headers 411 and 444 (sixteen at state + 0xDC4): the
 * spine and a copy of it moved along the view, both projected, per point the
 * screen angle, the projected width, the colours, the age, the depth, the
 * width's screen offset and the growth. */
typedef struct {
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    CVECTOR outer;
    CVECTOR inner;
    s32 age;
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    s32 grow[2];
} Beam411;


#endif

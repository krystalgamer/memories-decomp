#ifndef MEMORIES_DECOMP_MODEL_VARIANT411_BEAMS_H
#define MEMORIES_DECOMP_MODEL_VARIANT411_BEAMS_H

#include "model_variant.h"

/* One 0x64-byte two-point beam of header 411 (sixteen at state + 0xDC4): the
 * spine and a copy of it moved along the view, both projected, per point the
 * screen angle, the projected width, the colours, the age, the depth, the
 * width's screen offset and the growth; and the state fields the beam helper
 * reads. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    u8 outer[4];
    u8 inner[4];
    s32 age;
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    s32 grow[2];
} Beam411;

typedef struct {
    u8 unknown_0000[0x54C];
    ModelVariantSheet sheet;
    u8 unknown_05E4[0x7E0];
    Beam411 beams[16];
    u8 unknown_1404[0x40];
    POLY_GT4 quad;
    u8 unknown_1478[0x18C];
    s16 position[3];
    u8 unknown_160A[0x16];
    s32 direction[3];
    u8 unknown_162C[0xC];
    s32 frame;
    u8 unknown_163C[8];
    s32 step;
    u8 unknown_1648[0x2C];
    s32 spin;
    u8 unknown_1678[4];
    s32 phase;
} Beam411State;

#endif

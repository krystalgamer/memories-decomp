#ifndef FRENCH_MODEL_VARIANT400_STREAMERS_H
#define FRENCH_MODEL_VARIANT400_STREAMERS_H

#include "../../types.h"
#include "variant400_ribbons.h"

typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 unknown_220[0x44];
    CVECTOR color[17];
    u8 unknown_2A8[4];
    s32 depth[17];
    s16 ox[17];
    s16 oy[17];
} Model400Streamer;

typedef struct {
    s32 scale;
    u8 unknown_04[0x94];
} Model400Size;

void func_8013CDB8(u8 *ctx);
#endif

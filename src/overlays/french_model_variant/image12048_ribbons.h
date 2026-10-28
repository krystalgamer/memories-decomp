#ifndef FRENCH_MODEL_IMAGE12048_RIBBONS_H
#define FRENCH_MODEL_IMAGE12048_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    PSXLONG packed;
    DVECTOR point;
} Ribbon12048Screen;

typedef struct {
    SVECTOR points[9];
    Ribbon12048Screen screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    Ribbon12048Screen edge_screen[9];
    s32 width[9];
    u8 unknown_120[0x24];
    CVECTOR color[9];
    s32 cycles;
    s32 direction[3];
    u8 unknown_178[4];
    s32 first, last;
    s32 bearing[2];
    s32 wave[2];
    PSXLONG depth[9];
    u16 x_offset[9], y_offset[9];
} Ribbon12048;

typedef struct {
    u8 unknown_0000[0xA78];
    Ribbon12048 ribbons[3];
    u8 unknown_100C[0xA8];
    POLY_FT4 quads[2];
    u8 unknown_1104[0xD0];
    SVECTOR translation;
    u8 unknown_11DC[0x158];
    s32 view_direction[3];
    u8 unknown_1340[0x80];
    s32 phase;
} Ribbons12048State;

#endif

#ifndef FRENCH_MODEL_VARIANT463_RIBBONS_H
#define FRENCH_MODEL_VARIANT463_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon463Screen;

typedef struct {
    SVECTOR a[13];
    Ribbon463Screen sa[13];
    s32 angle[13];
    SVECTOR b[13];
    Ribbon463Screen sb[13];
    s32 width[13];
    u8 color[4];
    u8 unknown_1A4[0x1C];
    s32 count;
    u8 unknown_1C4[4];
    s32 state;
    s32 scale;
    u8 unknown_1D0[0x78];
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} Ribbon463;

typedef struct {
    Ribbon463 ribbons[5];
    u8 unknown_0D70[0x3660];
    POLY_FT4 polygons[2];
    u8 unknown_4420[0x124];
    VECTOR positions[5];
    u8 unknown_4594[0x50];
    VECTOR dirs[5];
} Ribbon463Work;

typedef struct {
    u8 unknown_0000[0xD70];
    Ribbon463 ribbons[5];
    u8 unknown_1AE0[0x28F0];
    POLY_FT4 polygons[2];
    u8 unknown_4420[0x214];
    VECTOR positions[5];
    u8 unknown_4684[0x50];
    VECTOR dirs[5];
} TrailRibbon463Work;

#endif

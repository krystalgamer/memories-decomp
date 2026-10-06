#ifndef SPANISH_MODEL_VARIANT477_QUADS_H
#define SPANISH_MODEL_VARIANT477_QUADS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[10];
    SVECTOR b[10];
    SVECTOR c[10];
    SVECTOR d[10];
    u8 unknown_140[0x50];
    u8 color[4];
    s32 size[10];
    s32 angle[10];
    s32 done[10];
    u8 unknown_20C[0xF0];
} QuadGroup477;

typedef struct {
    u8 unknown_0000[0x118];
    QuadGroup477 groups[2];
    u8 unknown_0710[0x188C];
    POLY_FT4 polygon;
    u8 unknown_1FC4[0x6C];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_204C[0x24];
    s32 step;
    u8 unknown_2074[0x28];
    s32 enabled[2];
    u8 unknown_20A4[4];
    s32 phase;
} Quads477State;

#endif

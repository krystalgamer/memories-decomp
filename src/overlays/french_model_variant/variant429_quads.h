#ifndef FRENCH429_QUADS_VIEW_H
#define FRENCH429_QUADS_VIEW_H

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
    u8 unknown_20C[0x28];
    s32 reset[10];
    u8 unknown_25C[0xA0];
} QuadGroup429;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Quads429Timing;

typedef struct {
    QuadGroup429 groups[1];
    u8 unknown_02FC[0x19F0];
    POLY_FT4 polygon;
    u8 unknown_1D14[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_1D74[0xDC];
    u32 time;
    u8 unknown_1E54[4];
    s32 step;
    u8 unknown_1E5C[4];
    Quads429Timing *G32 timing;
    u8 unknown_1E64[0x24];
    s32 phase;
} Quads429State;

#endif

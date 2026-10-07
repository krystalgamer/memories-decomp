#ifndef FRENCH449_SHEET_VIEW_H
#define FRENCH449_SHEET_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[4][4];
    CVECTOR color[2];
} Variant449Sheet;

typedef struct {
    u8 unknown_00[0x10];
    s32 scale_start;
    s32 scale_end;
    s32 move_start;
    s32 move_end;
    s32 fade_start;
} Variant449Timing;

typedef struct {
    u8 unknown_0000[0xE00];
    Variant449Sheet sheets[1];
    u8 unknown_0E88[0xB68];
    POLY_GT4 quads[2];
    u8 unknown_1A58[0xBC];
    MATRIX origin;
    u8 unknown_1B34[0x5C];
    s32 factor;
    u8 unknown_1B94[4];
    s32 scale;
    u8 unknown_1B9C[0x14];
    VECTOR delta;
    u8 unknown_1BC0[4];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1BD0[12];
    s32 flags;
    s32 elapsed;
    u8 unknown_1BE4[4];
    s32 step;
    u8 unknown_1BEC[8];
    Variant449Timing *G32 timing;
    u8 unknown_1BF8[0x44];
    s32 phase;
} Variant449View;

#endif

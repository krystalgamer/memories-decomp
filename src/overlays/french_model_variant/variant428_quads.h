#ifndef FRENCH428_QUADS_VIEW_H
#define FRENCH428_QUADS_VIEW_H

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
} QuadGroup428;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Quads428Timing;

typedef struct {
    QuadGroup428 groups[1];
    u8 unknown_02FC[0x1FA0];
    POLY_FT4 polygon;
    u8 unknown_22C4[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2324[0x41C];
    u32 time;
    u8 unknown_2744[4];
    s32 step;
    u8 unknown_274C[4];
    Quads428Timing *G32 timing;
    u8 unknown_2754[0x24];
    s32 phase;
} Quads428State;

#endif

#ifndef FRENCH419_QUADS_VIEW_H
#define FRENCH419_QUADS_VIEW_H

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
} QuadGroup419;

typedef struct {
    u8 unknown_00[0x24];
    u32 limit;
} Quads419Timing;

typedef struct {
    QuadGroup419 groups[1];
    u8 unknown_02FC[0x1F20];
    POLY_FT4 polygon;
    u8 unknown_2244[0x44];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_22A4[0x1C];
    u32 time;
    u8 unknown_22C4[4];
    s32 step;
    u8 unknown_22CC[4];
    Quads419Timing *G32 timing;
    u8 unknown_22D4[0x20];
    s32 phase;
} Quads419State;

#endif

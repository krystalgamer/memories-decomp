#ifndef FRENCH471_QUADS_VIEW_H
#define FRENCH471_QUADS_VIEW_H

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
} QuadGroup471;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Quads471Timing;

typedef struct {
    u8 unknown_0000[0x794];
    QuadGroup471 groups[1];
    u8 unknown_0A90[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2344[0x6C];
    SVECTOR origin;
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_23CC[0x1C];
    u32 time;
    u8 unknown_23EC[4];
    s32 step;
    u8 unknown_23F4[4];
    Quads471Timing *G32 timing;
    u8 unknown_23FC[0x30];
    s32 phase;
} Quads471State;

#endif

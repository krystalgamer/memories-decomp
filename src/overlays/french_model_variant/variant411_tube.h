#ifndef FRENCH411_TUBE_VIEW_H
#define FRENCH411_TUBE_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[9][9];
    u8 unknown_288[0x288];
    CVECTOR colors[9];
    u8 unknown_534[0x18];
} Tube411;

typedef struct {
    u8 unknown_00[0x10];
    s32 expansion_start;
    s32 expansion_end;
    s32 fade_start;
    s32 fade_end;
} Tube411Timing;

typedef struct {
    Tube411 tubes[1];
    u8 unknown_54C[0xED4];
    POLY_G4 quad;
    u8 unknown_1444[0x1B4];
    s32 origin[3];
    u8 unknown_1604[8];
    s32 direction[3];
    u8 unknown_1618[4];
    DVECTOR projected;
    s32 direction_b[3];
    u8 unknown_162C[0x10];
    u32 time;
    u8 unknown_1640[4];
    s32 step;
    u8 unknown_1648[4];
    Tube411Timing *G32 timing;
    u8 unknown_1650[8];
    s32 progress;
    u8 unknown_165C[0x10];
    s32 width;
    s32 angle;
    u8 unknown_1674[8];
    s32 phase;
} Tube411State;

#endif

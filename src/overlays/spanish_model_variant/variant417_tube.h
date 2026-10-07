#ifndef SPANISH_MODEL_VARIANT417_TUBE_H
#define SPANISH_MODEL_VARIANT417_TUBE_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[9][9];
    u8 unknown_288[0x288];
    CVECTOR colors[9];
    u8 unknown_534[0x18];
} Tube417;

typedef struct {
    u8 unknown_00[0x88];
    s32 size;
} Tube417Pulse;

typedef struct {
    u8 unknown_00[0x20];
    s32 expansion_start;
    s32 expansion_end;
    s32 fade_start;
    s32 fade_end;
    s32 width_scale;
} Tube417Timing;

/* MODEL441's state with every field 0x5AC bytes lower. */
typedef struct {
    Tube417 tubes[1];
    u8 unknown_54C[0x5E4];
    Tube417Pulse pulse;
    u8 unknown_BBC[0x13E0];
    POLY_G4 quad;
    u8 unknown_1FC0[0x124];
    s32 origin[3];
    u8 unknown_20F0[8];
    s32 direction[3];
    u8 unknown_2104[4];
    DVECTOR projected;
    s32 direction_b[3];
    u8 unknown_2118[0x10];
    s32 time;
    u8 unknown_212C[4];
    s32 step;
    u8 unknown_2134[4];
    Tube417Timing *G32 timing;
    u8 unknown_213C[0x1C];
    s32 width;
    s32 progress;
    s32 angle;
    u8 unknown_2164[0xC];
    s32 phase;
} Tube417State;

#endif

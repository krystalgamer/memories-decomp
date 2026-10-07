#ifndef FRENCH385_GRID_VIEW_H
#define FRENCH385_GRID_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[9][17];
    u8 colors[9][4];
    s32 size;
    u8 unknown_4F0[4];
    s32 done;
} Grid385Sheet;

typedef struct {
    u8 unknown_000[0x180];
    s32 progress;
    s32 offset;
    s32 state;
    u8 unknown_18C[0xA0];
} Grid385Pulse;

typedef struct {
    Grid385Sheet sheets[5];
    Grid385Pulse pulses[5];
    u8 unknown_23B4[0x1540];
    POLY_GT4 quad;
    u8 unknown_3928[0x1E8];
    VECTOR positions[5];
    s32 axis[3];
    u8 unknown_3B6C[0x70];
    s32 flags;
    u8 unknown_3BE0[8];
    s32 step;
    u8 unknown_3BEC[0x1C];
    s32 pulse;
    u8 unknown_3C0C[0x50];
    s32 phase;
} Grid385State;

#endif

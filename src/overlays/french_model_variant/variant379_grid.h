#ifndef FRENCH379_GRID_VIEW_H
#define FRENCH379_GRID_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[9][17];
    u8 colors[9][4];
    s32 size;
    u8 unknown_4F0[4];
    s32 done;
} Grid379Sheet;

typedef struct {
    u8 unknown_000[0x180];
    s32 progress;
    s32 offset;
    s32 state;
    u8 unknown_18C[0xC4];
} Grid379Pulse;

typedef struct {
    Grid379Sheet sheets[5];
    Grid379Pulse pulses[5];
    u8 unknown_2468[0x19D0];
    POLY_GT4 quad;
    u8 unknown_3E6C[0x20C];
    VECTOR positions[5];
    u8 unknown_40C8[0x9C];
    s32 flags;
    u8 unknown_4168[8];
    s32 step;
    u8 unknown_4174[0x24];
    s32 pulse;
    u8 unknown_419C[0x50];
    s32 phase;
} Grid379State;

#endif

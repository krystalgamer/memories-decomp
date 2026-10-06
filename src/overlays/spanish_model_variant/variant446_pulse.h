#ifndef SPANISH_MODEL_VARIANT446_PULSE_H
#define SPANISH_MODEL_VARIANT446_PULSE_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[4][4];
    s32 size;
    u8 unknown_84[0xC];
} Pulse446;

typedef struct {
    u8 unknown_00[0xC];
    u16 count;
    u8 unknown_0E[0x1E];
    u32 start;
    u32 end;
    u8 unknown_34[4];
    u32 fade_start;
    u32 fade_end;
} Pulse446Timing;

typedef struct {
    u8 unknown_00[0x1130];
    Pulse446 groups[2];
    u8 unknown_1250[0x794];
    POLY_GT4 quad;
    u8 unknown_1A18[0x118];
    s32 origin[3];
    u8 unknown_1B3C[0x28];
    s32 direction[3];
    u8 unknown_1B70[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1B98[4];
    s32 step;
    u8 unknown_1BA0[4];
    Pulse446Timing *G32 timing;
    u8 unknown_1BA8[0x10];
    s32 index;
    u8 unknown_1BBC[0x12];
    s16 progress;
    u8 unknown_1BD0[0xC];
    s32 phase;
} Pulse446State;


#endif

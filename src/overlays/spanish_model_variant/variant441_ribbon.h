#ifndef SPANISH_MODEL_VARIANT441_RIBBON_H
#define SPANISH_MODEL_VARIANT441_RIBBON_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR points[3][2];
    PSXLONG projected[3][2];
    u8 unknown_48[0x1C];
    s32 depth[2];
} Ribbon441;

typedef struct {
    u8 unknown_00[0x18];
    u16 count;
    u8 unknown_1A[6];
    s32 expansion_start;
    s32 expansion_end;
    s32 fade_start;
    s32 fade_end;
} Ribbon441Timing;

typedef struct {
    u8 unknown_00[0xFD8];
    Ribbon441 ribbons[1];
    u8 unknown_1044[0x1528];
    POLY_GT4 quad;
    u8 unknown_25A0[0xF0];
    s32 origin[3];
    u8 unknown_269C[8];
    s32 direction[3];
    u8 unknown_26B0[4];
    DVECTOR projected;
    u8 unknown_26B8[0x1C];
    s32 time;
    u8 unknown_26D8[0xC];
    Ribbon441Timing *G32 timing;
    u8 unknown_26E8[0x10];
    s32 index;
    u8 unknown_26FC[0x18];
    s16 progress;
    s16 width;
    u8 unknown_2718[4];
    s32 phase;
} Ribbon441State;

#endif

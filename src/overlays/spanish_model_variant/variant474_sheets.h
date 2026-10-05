#ifndef SPANISH_MODEL_VARIANT474_SHEETS_H
#define SPANISH_MODEL_VARIANT474_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x18];
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_20[8];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_30[4];
} Sheets474Timing;

typedef struct {
    u8 unknown_0000[0x1198];
    ModelVariantSheet sheets[2];
    u8 unknown_12C8[0x7E0];
    POLY_GT4 polygon;
    u8 unknown_1ADC[0x74];
    s32 origin[3];
    u8 unknown_1B5C[8];
    s32 direction[3];
    u8 unknown_1B70[0x20];
    u32 frame;
    u32 time;
    u8 unknown_1B98[4];
    u32 step;
    u8 unknown_1BA0[4];
    Sheets474Timing *G32 timing;
    u8 unknown_1BA8[0x38];
    s32 displacement;
    u8 unknown_1BE4[0x38];
    s32 phase;
} Sheets474State;

#endif

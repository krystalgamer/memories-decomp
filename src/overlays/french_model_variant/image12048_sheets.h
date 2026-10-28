#ifndef FRENCH_MODEL_IMAGE12048_SHEETS_H
#define FRENCH_MODEL_IMAGE12048_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x28];
    s32 count;
    s32 divisor;
    u32 grow_end;
    u32 shrink_begin;
    u32 shrink_end;
} Sheets12048Timing;

typedef struct {
    u8 unknown_000[0x780];
    ModelVariantSheet sheets[14];
    u8 unknown_FD0[0x7C];
    POLY_GT4 quads[2];
    u8 unknown_10B4[0x114];
    s32 origin[3];
    SVECTOR offset;
    MATRIX transforms[11];
    u8 unknown_133C[0x10];
    u32 frame;
    u32 time;
    u8 unknown_1354[4];
    s32 step;
    u32 fade_span;
    u32 shrink_span;
    u8 unknown_1364[4];
    Sheets12048Timing *G32 timing;
    u8 unknown_136C[0x54];
    s32 phase;
} Sheets12048State;

#endif

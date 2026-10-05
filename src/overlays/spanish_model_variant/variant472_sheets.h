#ifndef SPANISH_MODEL_VARIANT472_SHEETS_H
#define SPANISH_MODEL_VARIANT472_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1C];
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_24[4];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_30[4];
} Sheets472Timing;

typedef struct {
    u8 unknown_0000[0x2B24];
    ModelVariantSheet sheets[1];
    u8 unknown_2BBC[0x8C];
    POLY_GT4 polygon;
    u8 unknown_2C7C[0x208];
    MATRIX world_matrix;
    u8 unknown_2EA4[0x44];
    u32 frame;
    u32 time;
    u8 unknown_2EF0[0x0C];
    Sheets472Timing *timing;
    u8 unknown_2F00[0x40];
    s32 phase;
} Sheets472State;

#endif

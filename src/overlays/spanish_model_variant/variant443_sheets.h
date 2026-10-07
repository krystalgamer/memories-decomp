#ifndef SPANISH_MODEL_VARIANT443_SHEETS_H
#define SPANISH_MODEL_VARIANT443_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x38];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_40[4];
    u32 spread_start;
    u32 spread_end;
    u8 unknown_4C[4];
    u32 fade_start;
    u32 fade_end;
} Timing443;

typedef struct {
    u8 unknown_0000[0x2A68];
    ModelVariantSheet sheets[5];
    u8 unknown_2D60[0x6A4];
    POLY_GT4 quad;
    u8 unknown_3438[0x138];
    VECTOR positions[3];
    s32 origin[3];
    u8 unknown_35AC[4];
    s16 rest[3];
    u8 unknown_35B6[2];
    s32 direction[3];
    u8 unknown_35C4[0x5C];
    u32 frame;
    u32 time;
    u8 unknown_3628[0xC];
    Timing443 *G32 timing;
    u8 unknown_3638[0x1C];
    s32 progress;
    u8 unknown_3658[0x24];
    s32 travel;
    u8 unknown_3680[0x18];
    s32 phase;
} Sheets443State;

#endif

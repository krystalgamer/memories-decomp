#ifndef SPANISH_MODEL_VARIANT461_SHEETS_H
#define SPANISH_MODEL_VARIANT461_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1C];
    s32 first_count;
    s32 paired;
    u8 unknown_24[0x0C];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_38[8];
} Sheets461Timing;

typedef struct {
    u8 unknown_00[0x188];
    s32 active;
    s32 size;
    u8 unknown_190[0xC0];
} Sheets461Primary;

typedef struct {
    Sheets461Primary primary[5];
    u8 unknown_0B90[0x15B4];
    ModelVariantSheetSet sheets[10];
    u8 unknown_275C[0x670];
    POLY_GT4 polygons[2];
    u8 unknown_2E34[0x108];
    VECTOR positions[5];
    VECTOR other_positions[5];
    u8 unknown_2FDC[0x37C];
    u32 frame;
    u32 time;
    u8 unknown_3360[4];
    u32 step;
    u8 unknown_3368[4];
    Sheets461Timing *G32 timing;
    u8 unknown_3370[0x3C];
    s32 phase;
} Sheets461State;

#endif

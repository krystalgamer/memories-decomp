#ifndef SPANISH_MODEL_VARIANT469_SHEETS_H
#define SPANISH_MODEL_VARIANT469_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_0000[0xA84];
    ModelVariantSheetPrimary primary[5];
    u8 unknown_1614[0x10D4];
    ModelVariantSheetSet sheets[10];
    u8 unknown_2D00[0x688];
    POLY_GT4 polygons[2];
    u8 unknown_33F0[0x168];
    VECTOR positions[5];
    VECTOR other_positions[5];
    u8 unknown_35F8[0x38C];
    u32 frame;
    u32 time;
    u8 unknown_398C[4];
    u32 step;
    u8 unknown_3994[4];
    ModelVariantPairedSheetTiming *timing;
    u8 unknown_399C[0x3C];
    s32 phase;
} Sheets469State;

#endif

#ifndef SPANISH_MODEL_VARIANT473_SHEETS_H
#define SPANISH_MODEL_VARIANT473_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_000[0x40];
    SVECTOR position;
    u8 unknown_048[0x140];
    s32 active;
    u8 unknown_18C[0xA0];
} Sheets473Primary;

typedef struct {
    u8 unknown_0000[0x18D8];
    Sheets473Primary primary[5];
    u8 unknown_23B4[0x5D0];
    ModelVariantSheet sheets[6];
    u8 unknown_2D14[0xCF0];
    POLY_GT4 polygon;
    u8 unknown_3A38[0xC4];
    VECTOR origin;
    u8 unknown_3B0C[0xD0];
    u32 frame;
    u32 time;
    u8 unknown_3BE4[4];
    u32 step;
    u8 unknown_3BEC[4];
    ModelVariantSheetTiming *timing;
    u8 unknown_3BF4[0x68];
    s32 phase;
} Sheets473State;

#endif

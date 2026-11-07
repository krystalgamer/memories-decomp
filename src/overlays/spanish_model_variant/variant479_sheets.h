#ifndef SPANISH_MODEL_VARIANT479_SHEETS_H
#define SPANISH_MODEL_VARIANT479_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_000[0x40];
    SVECTOR position;
    u8 unknown_048[0x140];
    s32 active;
    u8 unknown_18C[0xC4];
} Sheets479Primary;

typedef struct {
    u8 unknown_0000[0x18D8];
    Sheets479Primary primary[5];
    u8 unknown_2468[0x930];
    ModelVariantSheet sheets[8];
    u8 unknown_3258[0xD24];
    POLY_GT4 polygon;
    u8 unknown_3FB0[0x74];
    ModelVariantAnchor positions[3];
    u8 unknown_4084[0xE0];
    u32 frame;
    u32 time;
    u8 unknown_416C[4];
    u32 step;
    u8 unknown_4174[4];
    ModelVariantSheetTiming *timing;
    u8 unknown_417C[0x70];
    s32 phase;
} Sheets479State;

#endif

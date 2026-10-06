#ifndef SPANISH_MODEL_VARIANT459_SHEETS_H
#define SPANISH_MODEL_VARIANT459_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x24];
    u16 count;
    u8 unknown_26[2];
    u16 limit;
    u8 unknown_2A[0xA];
    u32 grow_start;
    u32 grow_end;
} Timing459;

typedef struct {
    u8 unknown_00[0x2568];
    ModelVariantSheet sheets[13];
    u8 unknown_2D20[0x74];
    POLY_GT4 quad;
    u8 unknown_2DC8[0x1C4];
    VECTOR origins[6];
    VECTOR paths[6];
    SVECTOR center;
    u8 unknown_3054[0x2C];
    u32 frame;
    u32 time;
    u8 unknown_3088[4];
    u32 step;
    u8 unknown_3090[0xC];
    Timing459 *G32 timing;
    u8 unknown_30A0[0x2C];
    s16 scale;
    u8 unknown_30CE[2];
    s32 progress;
    u8 unknown_30D4[8];
    s32 phase;
} Sheet459State;

#endif

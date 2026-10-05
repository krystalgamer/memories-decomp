#ifndef SPANISH_MODEL_VARIANT423_SHEETS_H
#define SPANISH_MODEL_VARIANT423_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[8];
    u32 shrink_start;
    u32 shrink_end;
} Sheet425Descriptor;

typedef struct {
    u8 unknown_0000[0xE1C];
    ModelVariantSheet sheets[2];
    u8 unknown_0F4C[0x754];
    POLY_GT4 polygon;
    u8 unknown_16D4[0x74];
    s32 origin[3];
    u8 unknown_1754[8];
    s32 direction[3];
    u8 unknown_1768[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1790[4];
    s32 step;
    u8 unknown_1798[4];
    Sheet425Descriptor *G32 descriptor;
    u8 unknown_17A0[0x30];
    s32 progress;
    u8 unknown_17D4[0x38];
    s32 phase;
} Sheet425State;

#endif

#ifndef SPANISH_MODEL_VARIANT423_SHEETS_H
#define SPANISH_MODEL_VARIANT423_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[4];
    u32 shrink_start;
    u32 shrink_end;
} Sheet423Descriptor;

typedef struct {
    u8 unknown_0000[0xE1C];
    ModelVariantSheet sheets[2];
    u8 unknown_0F4C[0xD58];
    POLY_GT4 polygon;
    u8 unknown_1CD8[0x74];
    s32 origin[3];
    u8 unknown_1D58[8];
    s32 direction[3];
    u8 unknown_1D6C[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1D94[4];
    s32 step;
    u8 unknown_1D9C[4];
    Sheet423Descriptor *G32 descriptor;
    u8 unknown_1DA4[0x30];
    s32 progress;
    u8 unknown_1DD8[0x34];
    s32 phase;
} Sheet423State;

#endif

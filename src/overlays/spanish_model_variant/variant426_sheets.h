#ifndef SPANISH_MODEL_VARIANT426_SHEETS_H
#define SPANISH_MODEL_VARIANT426_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x18];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_20[8];
    u32 shrink_start;
    u32 shrink_end;
} Sheet426Descriptor;

typedef struct {
    u8 unknown_0000[0xE1C];
    ModelVariantSheet sheets[2];
    u8 unknown_0F4C[0x1078];
    POLY_GT4 polygon;
    u8 unknown_1FF8[0xC4];
    s32 origin[3];
    u8 unknown_20C8[8];
    s32 direction[3];
    u8 unknown_20DC[0x20];
    s32 frame;
    u32 time;
    u8 unknown_2104[4];
    s32 step;
    u8 unknown_210C[4];
    Sheet426Descriptor *G32 descriptor;
    u8 unknown_2114[0x30];
    s32 progress;
    u8 unknown_2148[0x38];
    s32 phase;
} Sheet426State;

#endif

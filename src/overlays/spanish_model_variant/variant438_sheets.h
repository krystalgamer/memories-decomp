#ifndef SPANISH_MODEL_VARIANT438_SHEETS_H
#define SPANISH_MODEL_VARIANT438_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1C];
    u32 grow_start;
    u32 grow_end;
} Sheet438Timing;

typedef struct {
    u8 unknown_0000[0x1938];
    ModelVariantSheet sheets[2];
    u8 unknown_1A68[0x6A4];
    POLY_GT4 polygon;
    u8 unknown_2140[0x118];
    s32 origin[3];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_2278[0x20];
    s32 frame;
    u32 time;
    u8 unknown_22A0[4];
    s32 step;
    u8 unknown_22A8[0xC];
    Sheet438Timing *G32 timing;
    u8 unknown_22B8[0x20];
    s16 fixed_size;
    u8 unknown_22DA[2];
    s32 progress;
    u8 unknown_22E0[8];
    s32 phase;
} Sheet438State;

#endif

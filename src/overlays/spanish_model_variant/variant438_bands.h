#ifndef SPANISH_MODEL_VARIANT438_BANDS_H
#define SPANISH_MODEL_VARIANT438_BANDS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x20];
    u32 grow_start;
    u32 grow_end;
    u32 fade_start;
    u32 fade_end;
    u8 unknown_30[0x10];
    s32 radius;
} Bands438Timing;

typedef struct {
    u8 unknown_0000[0x1770];
    ModelVariantBand bands[1];
    u8 unknown_1938[0x7A0];
    POLY_GT4 quad;
    u8 unknown_210C[0x14C];
    s32 origin[3];
    u8 unknown_2264[8];
    s32 direction[3];
    u8 unknown_2278[4];
    DVECTOR projected;
    u8 unknown_2280[0x18];
    u32 frame;
    u32 time;
    u8 unknown_22A0[0x14];
    Bands438Timing *G32 timing;
    u8 unknown_22B8[0x20];
    s16 size;
    u8 unknown_22DA[2];
    s32 progress;
    u8 unknown_22E0[8];
    s32 phase;
} Bands438State;

#endif

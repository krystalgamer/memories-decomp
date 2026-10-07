#ifndef SPANISH_MODEL_VARIANT456_SHEETS_H
#define SPANISH_MODEL_VARIANT456_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x14];
    u32 grow_start;
    u32 grow_end;
    u32 fade_start;
    u32 fade_end;
    u32 spread_start;
} Timing456;

typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    u8 unknown_8C[0xC];
    s32 position[3];
    u8 unknown_A4[0x14];
    s32 faded;
    s32 fade;
} Sheet456;

typedef struct {
    u8 unknown_0000[0x14C0];
    Sheet456 sheets[8];
    u8 unknown_1AC0[0x6DC];
    POLY_GT4 quad;
    u8 unknown_21D0[0x10C];
    s32 origin[3];
    u8 unknown_22E8[0x34];
    u32 frame;
    u32 time;
    u8 unknown_2324[4];
    u32 step;
    u8 unknown_232C[4];
    Timing456 *G32 timing;
    u8 unknown_2334[0x38];
    s32 phase;
} Sheets456State;

#endif

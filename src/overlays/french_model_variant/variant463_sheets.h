#ifndef FRENCH_MODEL_VARIANT463_SHEETS_H
#define FRENCH_MODEL_VARIANT463_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x20];
    s32 first_count;
    s32 paired;
    u8 unknown_28[8];
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_38[4];
    u32 fade_begin;
    u32 fade_end;
} Sheets463Timing;

typedef struct {
    u8 unknown_000[0x1C8];
    s32 active;
    s32 size;
    u8 unknown_1D0[0xE0];
} Sheets463Primary;

typedef struct {
    u8 unknown_0000[0xD70];
    Sheets463Primary primary[1];
    u8 unknown_1020[0x10D8];
    ModelVariantSheetSet sheets[10];
    u8 unknown_2710[0x1C24];
    POLY_GT4 polygons[2];
    u8 unknown_439C[0x298];
    VECTOR other_positions[5];
    VECTOR positions[5];
    u8 unknown_46D4[0x37C];
    u32 frame;
    u32 time;
    u8 unknown_4A58[0xC];
    Sheets463Timing *G32 timing;
    u8 unknown_4A68[0x48];
    s32 phase;
} Sheets463State;

#endif

#ifndef FRENCH_MODEL_VARIANT462_SHEETS_H
#define FRENCH_MODEL_VARIANT462_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x1C];
    s32 first_count;
    s32 paired;
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_2C[4];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_38[8];
} Sheets462Timing;

typedef struct {
    u8 unknown_000[0x1C8];
    s32 active;
    s32 size;
    u8 unknown_1D0[0xE0];
} Sheets462Primary;

typedef struct {
    Sheets462Primary primary[5];
    u8 unknown_0D70[0x15B4];
    ModelVariantSheetSet sheets[10];
    u8 unknown_293C[0x670];
    POLY_GT4 polygons[2];
    u8 unknown_3014[0x108];
    VECTOR positions[5];
    VECTOR other_positions[5];
    u8 unknown_31BC[0x37C];
    u32 frame;
    u32 time;
    u8 unknown_3540[4];
    u32 step;
    u8 unknown_3548[4];
    Sheets462Timing *G32 timing;
    u8 unknown_3550[0x3C];
    s32 phase;
} Sheets462State;

#endif

#ifndef FRENCH_MODEL_VARIANT463_PAIRED_SHEETS_H
#define FRENCH_MODEL_VARIANT463_PAIRED_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x20];
    s32 first_count;
    s32 paired;
    u8 unknown_28[4];
    s32 mode;
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_38[4];
    u32 fade_begin;
    u32 fade_end;
} PairedSheets463Timing;

typedef struct {
    u8 unknown_000[0x1C8];
    s32 active;
    s32 size;
    u8 unknown_1D0[0xE0];
} PairedSheets463Primary;

typedef struct {
    PairedSheets463Primary primary[1];
    u8 unknown_02B0[0x1830];
    ModelVariantSheetSet sheets[10];
    u8 unknown_20F8[0x223C];
    POLY_GT4 polygons[2];
    u8 unknown_439C[0x1A8];
    VECTOR positions[5];
    VECTOR other_positions[5];
    u8 unknown_45E4[0x46C];
    u32 frame;
    u32 time;
    u8 unknown_4A58[0xC];
    PairedSheets463Timing *G32 timing;
    u8 unknown_4A68[0x48];
    s32 phase;
} PairedSheets463State;

#endif

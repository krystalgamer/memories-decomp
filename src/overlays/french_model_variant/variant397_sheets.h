#ifndef FRENCH_MODEL_VARIANT397_SHEETS_H
#define FRENCH_MODEL_VARIANT397_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x24];
    s32 count;
    s32 divisor;
    u32 grow_begin;
    u32 grow_end;
    u32 fade_at;
} Sheets397Timing;

typedef struct {
    VECTOR position;
    u8 unknown_10[0x10];
} Sheets397Anchor;

typedef struct {
    u8 unknown_0000[0xE24];
    ModelVariantSheetSet sheets[10];
    u8 unknown_143C[0x458];
    POLY_GT4 quad;
    u8 unknown_18C8[0x114];
    Sheets397Anchor anchors[4];
    u8 unknown_1A5C[0x14];
    VECTOR positions[4];
    u8 unknown_1AB0[0x8C];
    u32 frame;
    u32 time;
    u8 unknown_1B44[4];
    s32 step;
    u8 unknown_1B4C[8];
    Sheets397Timing *G32 timing;
    u8 unknown_1B58[0xA0];
    s32 phase;
} Sheets397State;

#endif

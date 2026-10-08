#ifndef FRENCH_MODEL_VARIANT389_SHEETS_H
#define FRENCH_MODEL_VARIANT389_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x18];
    s32 paired;
    s32 single;
    u8 unknown_20[0xC];
    u32 grow_begin;
    u32 grow_end;
    u32 fade_begin;
    u32 fade_end;
} Sheets389Timing;

typedef struct {
    u8 unknown_000[0x39C];
    s32 level;
    u8 unknown_3A0[0x44];
} Sheets389Primary;

typedef struct {
    Sheets389Primary primary[8];
    ModelVariantSheetSet sheets[16];
    u8 unknown_28E0[0x40];
    POLY_GT4 quads[2];
    u8 unknown_2988[0xD4];
    MATRIX transforms[8];
    VECTOR positions[8];
    u8 unknown_2BDC[0xAC];
    u32 frame;
    u32 time;
    u8 unknown_2C90[4];
    s32 step;
    u8 unknown_2C98[4];
    Sheets389Timing *G32 timing;
    u8 unknown_2CA0[0x40];
    s32 phase;
} Sheets389State;

#endif

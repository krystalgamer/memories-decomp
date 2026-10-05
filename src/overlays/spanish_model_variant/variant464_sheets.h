#ifndef SPANISH_MODEL_VARIANT464_SHEETS_H
#define SPANISH_MODEL_VARIANT464_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0x80];
    s32 active;
} Sheets464Primary;

typedef struct {
    u8 unknown_00[0x1C];
    u32 grow_begin;
    u32 grow_end;
    u8 unknown_24[4];
    u32 fade_begin;
    u32 fade_end;
    u8 unknown_30[0x14];
    s32 count;
} Sheets464Timing;

typedef struct {
    u8 unknown_0000[0x4E0];
    Sheets464Primary primary[5];
    ModelVariantSheetSet sheets[6];
    u8 unknown_0B1C[0xF98];
    POLY_GT4 polygon;
    u8 unknown_1AE8[0xF0];
    s32 origin[3];
    SVECTOR anchor;
    u8 unknown_1BEC[0x2C];
    u32 frame;
    u32 time;
    u8 unknown_1C20[4];
    u32 step;
    u8 unknown_1C28[4];
    Sheets464Timing *G32 timing;
    u8 unknown_1C30[0x30];
    s32 phase;
} Sheets464State;

#endif

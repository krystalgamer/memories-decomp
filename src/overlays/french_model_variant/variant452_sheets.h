#ifndef FRENCH_MODEL_VARIANT452_SHEETS_H
#define FRENCH_MODEL_VARIANT452_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown00[0x1C];
    u32 scale_start;
    u32 scale_end;
    u8 unknown24[8];
    u32 fade_start;
    u32 fade_end;
} Variant452SheetTiming;

/* Minimum observed view, not an allocation-capacity declaration. */
typedef struct {
    u8 unknown0000[0x2AC8];
    ModelVariantSheet sheets[1];
    u8 unknown2B60[0x6A4];
    POLY_GT4 poly;
    u8 unknown3238[0xD0];
    s32 translation[3];
    u8 unknown3314[0x40];
    s32 flags;
    u32 elapsed;
    u8 unknown335C[4];
    s32 step;
    u8 unknown3364[0x10];
    Variant452SheetTiming *G32 timing;
    u8 unknown3378[0x1C];
    s32 field3394;
    u8 unknown3398[0x10];
    s32 phase;
} Variant452SheetView;

#endif

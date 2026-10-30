#ifndef FRENCH_MODEL_VARIANT423_RIBBON_H
#define FRENCH_MODEL_VARIANT423_RIBBON_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon423Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon423Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon423Screen sb[17];
    s32 width[17];
    u8 unknown220[0xB8];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Ribbon423;

typedef struct {
    u8 unknown00[0x24];
    u32 fade_start;
    u32 fade_end;
} Ribbon423Timing;

typedef struct {
    u8 unknown0000[0x4EC];
    Ribbon423 ribbon[1];
    u8 unknown084C[0x1C70 - 0x84C];
    POLY_GT4 poly;
    u8 unknown1CA4[0x1D4C - 0x1CA4];
    s32 origin[3];
    u8 unknown1D58[8];
    s32 vec[3];
    u8 unknown1D6C[8];
    s32 target[3];
    u8 unknown1D80[0xC];
    u32 frame;
    u32 clock;
    s32 unknown1D94;
    s32 step;
    s32 unknown1D9C;
    Ribbon423Timing *G32 timing;
    u8 unknown1DA4[0xC];
    s32 fade;
    u8 unknown1DB4[0x20];
    s32 amp;
    u8 unknown1DD8[0x24];
    s32 phase;
    s32 wave;
    u8 unknown1E04[8];
    s32 state;
} Ribbon423Work;
#endif

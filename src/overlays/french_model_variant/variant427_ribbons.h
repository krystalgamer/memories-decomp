#ifndef FRENCH_MODEL_VARIANT427_RIBBONS_H
#define FRENCH_MODEL_VARIANT427_RIBBONS_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon379Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon379Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon379Screen sb[17];
    s32 width[17];
    u8 unknown220[0x20];
    s32 amp;
    u8 unknown244[0x98];
    PSXLONG flags[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Ribbon379;

typedef struct {
    VECTOR position;
    u8 unknown10[0x10];
} Ribbon379Anchor;

typedef struct {
    u8 unknown00[0x24];
    u32 fade_start;
    u32 fade_end;
} Ribbon379Timing;


typedef struct {
    u8 unknown0000[0x363C];
    Ribbon379Anchor anchor[3];
    u8 unknown369C[0x36F0 - 0x369C];
    VECTOR vec[3];
    u8 unknown3720[0x3740 - 0x3720];
    u32 clock;
    s32 unknown3744;
    s32 step;
    s32 unknown374C;
    Ribbon379Timing *G32 timing;
    u8 unknown3754[0x3768 - 0x3754];
    s32 fade;
    u8 unknown376C[0x37B4 - 0x376C];
    s32 phase;
    s32 wave;
    u8 unknown37BC[0x37C4 - 0x37BC];
    s32 state;
} Ribbon379Work;
#endif

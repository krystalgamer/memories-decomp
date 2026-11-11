#ifndef FRENCH_MODEL_VARIANT426_RIBBON_H
#define FRENCH_MODEL_VARIANT426_RIBBON_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    long packed;
} Ribbon426Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon426Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon426Screen sb[17];
    s32 width[17];
    u8 unknown220[0xB8];
    ModelVariantStripOffsets offsets;
} Ribbon426;

typedef struct {
    u8 unknown0000[0x4EC];
    Ribbon426 ribbon[1];
    u8 unknown084C[0x1F90 - 0x84C];
    POLY_GT4 poly;
    u8 unknown1FC4[0x20BC - 0x1FC4];
    s32 origin[3];
    u8 unknown20C8[8];
    s32 vec[3];
    u8 unknown20DC[8];
    s32 target[3];
    u8 unknown20F0[0xC];
    u32 frame;
    u32 clock;
    s32 unknown2104;
    s32 step;
    s32 unknown210C;
    ModelVariantRibbonGrowTiming *timing;
    u8 unknown2114[0xC];
    s32 fade;
    u8 unknown2124[0x20];
    s32 amp;
    u8 unknown2148[0x28];
    s32 phase;
    s32 wave;
    u8 unknown2178[8];
    s32 state;
} Ribbon426Work;
#endif

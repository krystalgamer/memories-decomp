#ifndef FRENCH_MODEL_VARIANT425_RIBBON_H
#define FRENCH_MODEL_VARIANT425_RIBBON_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon425Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon425Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon425Screen sb[17];
    s32 width[17];
    u8 unknown220[0xB8];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Ribbon425;

typedef struct {
    u8 unknown00[0x1C];
    u32 grow_start;
    u32 grow_end;
    u8 unknown24[4];
    u32 fade_start;
    u32 fade_end;
} Ribbon425Timing;

typedef struct {
    u8 unknown0000[0x4EC];
    Ribbon425 ribbon[1];
    u8 unknown084C[0x166C - 0x84C];
    POLY_GT4 poly;
    u8 unknown16A0[0x1748 - 0x16A0];
    s32 origin[3];
    u8 unknown1754[8];
    s32 vec[3];
    u8 unknown1768[8];
    s32 target[3];
    u8 unknown177C[0xC];
    u32 frame;
    u32 clock;
    s32 unknown1790;
    s32 step;
    s32 unknown1798;
    Ribbon425Timing *G32 timing;
    u8 unknown17A0[0xC];
    s32 fade;
    u8 unknown17B0[0x20];
    s32 amp;
    u8 unknown17D4[0x28];
    s32 phase;
    s32 wave;
    u8 unknown1804[8];
    s32 state;
} Ribbon425Work;
#endif

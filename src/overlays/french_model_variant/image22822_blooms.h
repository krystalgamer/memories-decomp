#ifndef FRENCH_MODEL_IMAGE22822_BLOOMS_H
#define FRENCH_MODEL_IMAGE22822_BLOOMS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[12];
    SVECTOR b[12];
    SVECTOR c[12];
    SVECTOR d[12];
    u8 unknown_180[0x60];
    u8 color[4];
    u8 unknown_1E4[0x10];
    s32 size[12];
    s32 done[12];
    u8 unknown_254[0x30];
    VECTOR origin;
    VECTOR direction;
} Bloom22822Group;

typedef struct {
    u8 unknown_00[0x20];
    u32 limit;
} Bloom22822Timing;

typedef struct {
    Bloom22822Group groups[8];
    u8 unknown_1520[0x188C];
    POLY_FT4 polygon;
    u8 unknown_2DD4[0x5C];
    VECTOR direction;
    u8 unknown_2E40[0x10];
    DVECTOR projected;
    u8 unknown_2E54[0x1C];
    u32 time;
    u8 unknown_2E74[4];
    s32 step;
    u8 unknown_2E7C[4];
    Bloom22822Timing *G32 timing;
    u8 unknown_2E84[0x20];
    s32 phase;
    u8 unknown_2EA8[0xC];
    s16 mirror;
} Bloom22822State;

#endif

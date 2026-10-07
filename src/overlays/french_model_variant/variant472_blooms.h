#ifndef FRENCH_MODEL_VARIANT472_BLOOMS_H
#define FRENCH_MODEL_VARIANT472_BLOOMS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

/* One 0x2B4-byte group of twelve blooming quads. */
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
    u8 unknown_294[0x10];
    VECTOR direction;
} Bloom472Group;

typedef struct {
    u8 unknown_00[0x2C];
    u32 limit;
} Bloom472Timing;

typedef struct {
    Bloom472Group groups[8];
    u8 unknown_15A0[0x1898];
    POLY_FT4 polygon;
    u8 unknown_2E60[0x5C];
    VECTOR direction;
    DVECTOR projected;
    u8 unknown_2ED0[0x1C];
    u32 time;
    u8 unknown_2EF0[4];
    s32 step;
    u8 unknown_2EF8[4];
    Bloom472Timing *G32 timing;
    u8 unknown_2F00[0x40];
    s32 phase;
} Bloom472State;

#endif

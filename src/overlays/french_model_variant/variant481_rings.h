#ifndef FRENCH_MODEL_VARIANT481_RINGS_H
#define FRENCH_MODEL_VARIANT481_RINGS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    s32 scale;
    u8 unknown_114[4];
} Ring481;

typedef struct {
    u8 unknown_000[0x4A4];
    s32 level;
} Ring481Gauge;

typedef struct {
    Ring481 rings[2];
    Ring481Gauge gauge;
    u8 unknown_6D8[0x172C];
    POLY_GT4 quad;
    u8 unknown_1E38[0x1C4];
    SVECTOR position;
    s32 direction[3];
    u8 unknown_2010[0x20];
    u32 frame;
    u8 unknown_2034[8];
    s32 step;
    u8 unknown_2040[0x60];
    CVECTOR inner;
    CVECTOR outer;
    s32 spin;
    u8 unknown_20AC[0x28];
    s16 mode;
} Rings481State;

#endif

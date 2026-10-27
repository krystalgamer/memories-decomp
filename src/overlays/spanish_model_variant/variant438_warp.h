#ifndef SPANISH_MODEL_VARIANT438_WARP_H
#define SPANISH_MODEL_VARIANT438_WARP_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR outer[17];
    SVECTOR inner[17];
    SVECTOR source[17];
    CVECTOR inner_color;
    CVECTOR outer_color;
    s32 size;
    u8 unknown_1A4[4];
} Warp438Ring;

typedef struct {
    Warp438Ring rings[5];
    u8 unknown_0848[0x192C];
    POLY_GT4 polygon;
    u8 unknown_21A8[0xB0];
    s32 origin[3];
    u8 unknown_2264[8];
    s32 direction[3];
    u8 unknown_2278[0x2C];
    s32 step;
    u8 unknown_22A8[0x40];
    s32 phase;
} Warp438State;

typedef struct {
    Warp438Ring rings[5];
    u8 unknown_0848[0x2580];
    POLY_GT4 polygon;
    u8 unknown_2DFC[0xE4];
    s32 origin[3];
    u8 unknown_2EEC[0x168];
    s32 direction[3];
    u8 unknown_3060[0x2C];
    s32 step;
    u8 unknown_3090[0x4C];
    s32 phase;
} Warp459State;

#endif

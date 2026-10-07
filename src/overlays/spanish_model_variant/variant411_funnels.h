#ifndef SPANISH_MODEL_VARIANT411_FUNNELS_H
#define SPANISH_MODEL_VARIANT411_FUNNELS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_00[0xC];
    u32 grow_start;
    u32 grow_end;
    u8 unknown_14[4];
    u32 fade_after;
    u32 fade_end;
} Timing411Funnel;

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} Funnel411;

typedef struct {
    u8 unknown_000[0x54C];
    ModelVariantSheet sheet;
    u8 unknown_5E4[0x98];
    Funnel411 funnels[2];
    u8 unknown_8AC[0xC34];
    POLY_GT4 quad;
    u8 unknown_1514[0xE4];
    s32 origin[3];
    u8 unknown_1604[8];
    s32 direction[3];
    u8 unknown_1618[0x20];
    s32 frame;
    u32 time;
    u8 unknown_1640[4];
    s32 step;
    u8 unknown_1648[4];
    Timing411Funnel *G32 timing;
    u8 unknown_1650[0x10];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 spin;
    u8 unknown_166C[0x10];
    s32 phase;
} Funnel411State;

/* Start of the overlay data after the code; the fade start is read at +0x114. */
extern u8 D_8013D9B4[];

#endif

#ifndef SPANISH_MODEL_VARIANT426_FUNNEL_H
#define SPANISH_MODEL_VARIANT426_FUNNEL_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17];
    SVECTOR outer[17];
    s32 size;
    u8 unknown_114[4];
} Funnel426;

typedef struct {
    u8 unknown_0000[0xE1C];
    ModelVariantSheet sheets[2];
    u8 unknown_0F4C[0xCD0];
    Funnel426 funnels[1];
    u8 unknown_1D34[0x14C];
    POLY_GT4 quad;
    u8 unknown_1EB4[0x214];
    s16 position[3];
    u8 unknown_20CE[2];
    s32 direction[3];
    u8 unknown_20DC[0x20];
    s32 frame;
    u8 unknown_2100[8];
    s32 step;
    u8 unknown_210C[0x1C];
    s32 size;
    u8 unknown_212C[0xC];
    s32 fade;
    u8 unknown_213C[0x28];
    u8 inner_color[4];
    u8 outer_color[4];
    s32 spin;
    u8 unknown_2170[0x10];
    s32 phase;
} Funnel426State;

#endif

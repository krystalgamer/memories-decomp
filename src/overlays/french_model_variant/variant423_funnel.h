#ifndef FRENCH_MODEL_VARIANT423_FUNNEL_H
#define FRENCH_MODEL_VARIANT423_FUNNEL_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    SVECTOR inner[17], outer[17];
    s32 size;
    u8 unknown114[4];
} Funnel423;

typedef struct {
    u8 unknown00[0xE1C];
    ModelVariantSheet sheets[2];
    u8 unknownF4C[0x15B4 - 0xF4C];
    Funnel423 funnels[4];
    u8 unknown1A14[0x1B60 - 0x1A14];
    POLY_GT4 quad;
    u8 unknown1B94[0x1D58 - 0x1B94];
    s16 position[3];
    u8 unknown1D5E[2];
    s32 direction[3];
    u8 unknown1D6C[0x1D8C - 0x1D6C];
    s32 frame;
    u8 unknown1D90[8];
    s32 step;
    u8 unknown1D9C[0x1DB8 - 0x1D9C];
    s32 size;
    u8 unknown1DBC[12];
    s32 fade;
    u8 unknown1DCC[0x1DF0 - 0x1DCC];
    u8 inner_color[4], outer_color[4];
    s32 spin;
    u8 unknown1DFC[16];
    s32 phase;
} Funnel423State;

#endif

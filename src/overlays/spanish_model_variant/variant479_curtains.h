#ifndef SPANISH_MODEL_VARIANT479_CURTAINS_H
#define SPANISH_MODEL_VARIANT479_CURTAINS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    u8 unknown_000[0x4EC];
    s32 size;
    u8 unknown_4F0[8];
} Curtains479Primary;

typedef struct {
    Curtains479Primary primary[5];
    u8 unknown_18D8[0x15E0];
    s32 pulse_scale;
    u8 unknown_2EBC[0xA04];
    ModelVariantCurtain curtains[5];
    u8 unknown_3E38[0x34];
    POLY_GT4 polygon;
    u8 unknown_3EA0[0x1D8];
    VECTOR positions[5];
    u8 unknown_40C8[0x9C];
    s32 frame;
    u8 unknown_4168[8];
    u32 step;
    u8 unknown_4174[0x24];
    s32 gate;
    u8 unknown_419C[0x34];
    u8 inner[4];
    u8 outer[4];
    s32 angle;
} Curtains479State;

#endif

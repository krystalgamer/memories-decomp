#ifndef FRENCH582_SHEETS_H
#define FRENCH582_SHEETS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/graphics_packet_submission.h"

typedef struct {
    SVECTOR points[3][13];
    u8 unknown_138[0x34];
    s32 progress;
    s32 state;
} Sheet129;

typedef struct {
    u8 unknown_0000[0x340];
    Sheet129 sheets[5];
    u8 unknown_0A84[0x28EC];
    POLY_F4 mask;
    u8 unknown_3388[0x68];
    POLY_GT4 quad;
    u8 unknown_3424[0xE4];
    VECTOR positions[5];
    u8 unknown_3558[0xF0];
    s32 direction[3];
    u8 unknown_3654[0x33C];
    s32 step;
    u8 unknown_3994[0x44];
    s32 phase;
} Sheets129State;

#endif

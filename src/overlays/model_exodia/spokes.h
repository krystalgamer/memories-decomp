#ifndef FRENCH_SPECIAL_SPOKES_H
#define FRENCH_SPECIAL_SPOKES_H
#include "../../types.h"
#include "ring.h"

typedef struct {
    u8 prefix[0x298];
    POLY_GT4 quad;
    u8 gap_2CC[0x3F0 - 0x2CC];
    VECTOR origin;
    u8 gap_400[0x438 - 0x400];
    VECTOR direction;
    u8 gap_448[8];
    u32 frame_count, frame;
    u8 gap_458[12];
    ExodiaRingTiming *timing;
    u8 gap_468[0x488 - 0x468];
    s32 scale, field_48C, width, angle;
} ExodiaSpokeState;

void func_8013BC0C(u8 *context);
#endif

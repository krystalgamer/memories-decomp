#ifndef FRENCH_SPECIAL_BEAM_H
#define FRENCH_SPECIAL_BEAM_H
#include "../../types.h"
#include "ring.h"

typedef struct {
    SVECTOR outer[17];
    SVECTOR center[17];
    SVECTOR inner[17];
    s32 projected[3][17];
    u8 gap_264[0x280 - 0x264];
    s32 flags[17];
    s32 depth[17];
} ExodiaBeam;

typedef struct {
    ExodiaBeam beams[1];
    u8 gap_308[0xAC0 - 0x308];
    POLY_GT4 quad;
    u8 gap_AF4[0xC2C - 0xAF4];
    SVECTOR origin;
    VECTOR direction;
    SVECTOR angles;
    u8 gap_C4C[0xC60 - 0xC4C];
    u32 frame_count;
    u32 frame;
    u32 field_C68;
    u32 step;
    u8 gap_C70[0xC98 - 0xC70];
    s16 distance;
    s16 width;
    u8 gap_C9C[0xCA4 - 0xC9C];
    s32 phase;
} ExodiaBeamState;

void func_8017C760(u8 *context);
#endif

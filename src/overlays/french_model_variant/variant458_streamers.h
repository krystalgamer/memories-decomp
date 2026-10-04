#ifndef FRENCH_MODEL_VARIANT458_STREAMERS_H
#define FRENCH_MODEL_VARIANT458_STREAMERS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

typedef struct {
    SVECTOR a[13];
    PSXLONG sa[13];
    s32 angle[13];
    SVECTOR b[13];
    PSXLONG sb[13];
    s32 width[13];
    u8 unknown_01A0[0x34];
    CVECTOR color[13];
    u8 unknown_0208[4];
    s32 depth[13];
    s16 ox[13];
    s16 oy[13];
} Variant458Streamer;

typedef struct {
    u8 unknown_0000[0x106C];
    Variant458Streamer streamers[3];
    u8 unknown_17C8[0x410];
    POLY_FT4 quads[2];
    u8 unknown_1C28[0x38];
    MATRIX origins[7];
    u8 unknown_1D40[0xC0];
    s32 factor;
    u8 unknown_1E04[4];
    s32 scale;
    u8 unknown_1E0C[0x14];
    VECTOR deltas[7];
    u8 unknown_1E90[0x64];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1F00[0xC];
    s32 flags;
    u8 unknown_1F10[0x54];
    s32 length;
    s32 rotation_x;
    s32 rotation_y;
    s32 rotation_z;
} Variant458StreamerView;
#endif

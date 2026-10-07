#ifndef FRENCH_MODEL_VARIANT436_STREAMERS_H
#define FRENCH_MODEL_VARIANT436_STREAMERS_H
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
} Streamer436;

/* MODEL436 draws the three streamers once, from one origin moved along a
 * delta by the factor and lowered by a quarter of the lift as the factor
 * falls. */
typedef struct {
    u8 unknown_0000[0x93C];
    Streamer436 streamers[3];
    u8 unknown_1098[0x410];
    POLY_FT4 quads[2];
    u8 unknown_14F8[0x10];
    MATRIX origin;
    u8 unknown_1528[0x68];
    s32 factor;
    s32 lift;
    s32 scale;
    u8 unknown_159C[4];
    VECTOR delta;
    u8 unknown_15B0[0x14];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_15D0[0xC];
    s32 flags;
    u8 unknown_15E0[0x44];
    s32 length;
    s32 rotation_x;
    s32 rotation_y;
    s32 rotation_z;
} Streamer436View;
#endif

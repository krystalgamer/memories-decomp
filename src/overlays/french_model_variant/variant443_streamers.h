#ifndef FRENCH_MODEL_VARIANT443_STREAMERS_H
#define FRENCH_MODEL_VARIANT443_STREAMERS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"
#include "../model_variant/model_variant.h"

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
} Variant443Streamer;

/* MODEL443 places the streamers at +0x11CC and drives the three passes from
 * a position table and the sizes of three sheets. */
typedef struct {
    u8 unknown_0000[0x11CC];
    Variant443Streamer streamers[3];
    u8 unknown_1928[0x1140];
    ModelVariantSheet sheets[3];
    u8 unknown_2C30[0x88C];
    POLY_FT4 quads[2];
    u8 unknown_350C[0x64];
    VECTOR positions[3];
    u8 unknown_35A0[0x68];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_3614[0xC];
    u32 flags;
    u8 unknown_3624[0x5C];
    s32 length;
    s32 rotation_x;
    s32 rotation_y;
    s32 rotation_z;
} Variant443StreamerView;
#endif

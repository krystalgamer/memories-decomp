#ifndef FRENCH449_STREAMERS_VIEW_H
#define FRENCH449_STREAMERS_VIEW_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

typedef struct {
    u8 unknown_0000[0xF3C];
    ModelVariantShortStreamer streamers[3];
    u8 unknown_1698[0x410];
    POLY_FT4 quads[1];
    u8 unknown_1AD0[0x44];
    MATRIX origin;
    u8 unknown_1B34[0x5C];
    s32 factor;
    u8 unknown_1B94[4];
    s32 scale;
    u8 unknown_1B9C[0x14];
    VECTOR delta;
    u8 unknown_1BC0[4];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1BD0[0xC];
    s32 flags;
    u8 unknown_1BE0[0x44];
    s32 length;
    s32 rotation_x;
    s32 rotation_y;
    s32 rotation_z;
} Variant449StreamerView;

void FrenchModelVariant449_DrawStreamers(u8 *ctx);
#endif

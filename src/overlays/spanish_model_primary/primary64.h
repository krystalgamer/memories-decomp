#ifndef MEMORIES_DECOMP_MODEL_PRIMARY64_H
#define MEMORIES_DECOMP_MODEL_PRIMARY64_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_graphics_state.h"
#include "../../game/func_80058E1C.h"
#include "../../game/screen_projection.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"

typedef struct {
    u8 r, g, b, count;
    s16 divisor;
    u8 parts[12];
    s16 widths[12];
    s16 heights[12];
    s16 jitter[12];
} ModelPrimary64Config;

typedef struct {
    ModelPrimary64Config *config;
    SVECTOR points[12];
    u32 textures[5];
    s32 frame;
} ModelPrimary64State;

extern VECTOR D_8013A994;
extern GsIMAGE D_8013A9A4[5];
extern ModelPrimary64Config D_8013AA30[];

#endif

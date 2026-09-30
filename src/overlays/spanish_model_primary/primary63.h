#ifndef MEMORIES_DECOMP_MODEL_PRIMARY63_H
#define MEMORIES_DECOMP_MODEL_PRIMARY63_H

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
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"

typedef struct {
    u8 r, g, b;
    u8 parts[3];
    s16 radius, other_radius, period;
} ModelPrimary63Config;

typedef struct {
    ModelPrimary63Config *G32 config;
    SVECTOR points[3][16];
    SVECTOR velocities[3][16];
    s32 frame;
} ModelPrimary63State;

extern VECTOR D_8013AA08;
extern ModelPrimary63Config D_8013AA18[];

#endif

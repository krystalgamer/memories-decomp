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
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"

typedef struct {
    u8 r, g, b;
    u8 parts[3];
    s16 radius, other_radius, period;
} ModelPrimary63Config;

typedef struct {
    ModelPrimary63Config *config;
    SVECTOR points[3][16];
    SVECTOR velocities[3][16];
    s32 frame;
} ModelPrimary63State;

extern VECTOR gModelPrimary63_Scale;
extern ModelPrimary63Config gModelPrimary63_Configs[];

#endif

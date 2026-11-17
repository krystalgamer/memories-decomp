#ifndef MEMORIES_DECOMP_MODEL_PRIMARY62_H
#define MEMORIES_DECOMP_MODEL_PRIMARY62_H

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
    u8 r, g, b, part;
    s16 size, radius, travel, count, field_0C, delay;
} ModelPrimary62Config;

typedef struct {
    ModelPrimary62Config *config;
    SVECTOR points[16];
    u8 frames[16];
    s32 texture;
    s32 delay;
} ModelPrimary62State;

extern VECTOR gModelPrimary62_Scale;
extern GsIMAGE gModelPrimary62_Image;
extern ModelPrimary62Config gModelPrimary62_Configs[];

#endif

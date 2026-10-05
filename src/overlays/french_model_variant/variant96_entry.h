#ifndef FRENCH_MODEL_VARIANT96_ENTRY_H
#define FRENCH_MODEL_VARIANT96_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 r, g, b, part;
    s16 outer_radius, inner_radius, segments, count;
    s16 duration, flash_duration, spacing, delay;
} Model96Config;

typedef struct {
    Model96Config *G32 config;
    u8 field_04[8];
    SVECTOR outer[65];
    SVECTOR inner[65];
    SVECTOR positions[128];
    s32 scales[128];
    SVECTOR rotation;
    u8 prepared;
    u8 field_A25[3];
    u32 texture[1];
    u8 completed;
    u8 field_A2D[3];
    s32 elapsed;
} Model96State;

extern const VECTOR D_8013BC4C;
extern GsIMAGE D_8013BC5C[];
extern Model96Config D_8013BC78[];
s32 func_8013B004(u8 *context, s32 command);

#endif

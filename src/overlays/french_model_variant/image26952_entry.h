#ifndef FRENCH_MODEL_IMAGE26952_ENTRY_H
#define FRENCH_MODEL_IMAGE26952_ENTRY_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_slot_properties.h"
#include "../../game/graphics_packet_submission.h"

typedef struct {
    u16 width, height, travel_duration, count;
    u16 radius, spread, spark_count, spark_size;
    u8 fade, unknown_11;
    u16 unknown_12, start, camera_delay;
} Image26952Config;

typedef struct {
    Image26952Config *G32 config;
    SVECTOR positions[8];
    SVECTOR velocities[8];
    SVECTOR rotations[8];
    SVECTOR targets[8];
    SVECTOR history[8][32];
    SVECTOR sparks[8][8];
    u32 elapsed;
    u16 spark_frames[8][8];
    u16 stage;
    u16 hit[8];
    u16 visible, launched;
    u16 length[8];
    u16 head[8];
    s32 ambient;
    s32 textures[2];
    u8 color[8];
} Image26952State;

typedef union {
    SVECTOR vector;
    ModelEffectAdjustment dimensions;
} Image26952Position;

extern VECTOR D_8013C544;
extern GsIMAGE D_8013C554[2];
extern Image26952Config D_8013C58C[];

#endif

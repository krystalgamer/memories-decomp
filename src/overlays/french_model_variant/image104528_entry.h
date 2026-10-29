#ifndef FRENCH_MODEL_IMAGE104528_ENTRY_H
#define FRENCH_MODEL_IMAGE104528_ENTRY_H

#include "../../types.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u16 red;
    u16 green;
    u16 blue;
    u16 flash_extent;
    u16 burst_extent;
    u16 spread;
    u16 fan_extent;
    u16 burst_interval;
    u16 fade;
    u16 unknown_12;
    u16 delay;
    u16 spawn_end;
    u16 camera_time;
} Image104528Config;

typedef struct {
    Image104528Config *G32 config;
    SVECTOR flashes[64];
    SVECTOR bursts[64];
    SVECTOR offsets[64][16];
    u16 frames[64];
    u16 elapsed;
    u16 updates;
    s32 ambient;
    u16 flash_count;
    u16 phase;
    u16 burst_count;
    u16 unknown_2492;
    s32 textures[2];
    u8 red[64];
    u8 green[64];
    u8 blue[64];
} Image104528State;

typedef union {
    SVECTOR vector;
    ModelEffectAdjustment dimensions;
} Image104528Rotation;

extern VECTOR D_8013C0C8;
extern GsIMAGE D_8013C0D8[2];
extern Image104528Config D_8013C110[];

#endif

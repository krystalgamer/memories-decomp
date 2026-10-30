#ifndef FRENCH_MODEL_IMAGE5996_ENTRY_H
#define FRENCH_MODEL_IMAGE5996_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 triangle_rgb[3];
    u8 quad_rgb[3];
    u8 part;
    u8 unknown_07;
    s16 triangle_height;
    s16 triangle_width;
    s16 unknown_0c;
    s16 inner_radius;
    s16 outer_radius;
    s16 quad_extent;
    s16 burst_duration;
    s16 triangle_count;
    s16 delay;
    s16 release_at;
    s16 burst_at;
} Image5996Config;

typedef struct {
    Image5996Config *G32 config;
    SVECTOR rotations[32];
    SVECTOR velocities[32];
    SVECTOR rings[66];
    s32 textures[3];
    u8 tick;
    u8 notified;
    u16 unknown_422;
    s32 elapsed;
} Image5996State;

typedef union {
    SVECTOR fourth;
    DVECTOR screens[66];
} Image5996Projection;

extern VECTOR D_8013BF44;
extern GsIMAGE D_8013BF54[3];
extern Image5996Config D_8013BFA8[];

#endif

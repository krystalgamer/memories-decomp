#ifndef FRENCH_MODEL_IMAGE40200_ENTRY_H
#define FRENCH_MODEL_IMAGE40200_ENTRY_H

#include "../../types.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 travel_rgb[3];
    u8 echo_rgb[3];
    u8 part;
    u8 count;
    s16 travel_extent;
    s16 spread;
    s16 wobble_height;
    s16 echo_extent;
    s16 travel_duration;
    s16 hold_duration;
    s16 echo_duration;
    s16 spacing;
    s16 delay;
    s16 unknown_1A;
} Image40200Config;

typedef struct {
    Image40200Config *G32 config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    SVECTOR echoes[64];
    u32 textures[2];
    u8 notified;
    u8 unknown_60D[3];
    s32 elapsed;
    s32 updates;
} Image40200State;

extern VECTOR D_8013C108;
extern GsIMAGE D_8013C118[2];
extern Image40200Config D_8013C150[];

#endif

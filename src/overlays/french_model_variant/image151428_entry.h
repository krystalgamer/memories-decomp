#ifndef FRENCH_MODEL_IMAGE151428_ENTRY_H
#define FRENCH_MODEL_IMAGE151428_ENTRY_H

#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 line_red, line_green, line_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 part, unknown_07;
    u16 spread, unknown_0A, half_size, rise;
    u16 delay, emission_end, camera_first_end, camera_second_end;
    s16 camera_first, camera_second, camera_third;
    u16 unknown_1E[3];
} Image151428Config;

typedef struct {
    Image151428Config *G32 config;
    SVECTOR lines[256][2];
    SVECTOR velocity;
    SVECTOR sparks[64];
    u32 elapsed;
    u16 spark_frames[64];
    u16 impact_frames[256];
    u16 phase;
    u16 impact_flags[256];
    u16 trail_ages[256];
    u16 live_count, batch_count, camera_mode;
    s32 ambient;
    s32 textures[1];
} Image151428State;

extern VECTOR D_8013C140;
extern GsIMAGE D_8013C150[1];
extern Image151428Config D_8013C16C[];

#endif

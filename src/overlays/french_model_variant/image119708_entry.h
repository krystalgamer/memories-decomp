#ifndef FRENCH_MODEL_IMAGE119708_ENTRY_H
#define FRENCH_MODEL_IMAGE119708_ENTRY_H

#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_update_coordinate_unit_matrix.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u16 half_size, count, fall_speed, unknown_06, spread, rise_frames;
    u32 scale_step, rise_end, pause_end, grow_end, hold_end, return_end, unknown_24;
} Image119708Config;

typedef struct {
    Image119708Config *G32 config;
    VECTOR offset, velocity;
    SVECTOR particles[128];
    u32 scale, elapsed;
    u16 phase, live_count;
    u16 speeds[128], ages[128];
    s32 ambient, textures[1];
} Image119708State;

extern VECTOR D_8013C2CC;
extern GsIMAGE D_8013C2DC[1];
extern Image119708Config D_8013C2F8[];

#endif

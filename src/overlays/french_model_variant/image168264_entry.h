#ifndef FRENCH_MODEL_IMAGE168264_ENTRY_H
#define FRENCH_MODEL_IMAGE168264_ENTRY_H

#include "../../types.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/model_effect_requests.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 ray_rgb[3];
    u8 particle_rgb[3];
    u8 parts[16];
    s16 radius_range;
    s16 ray_width;
    s16 radius_base;
    s16 tint_duration;
    s16 ray_duration;
    s16 particle_duration;
    s16 animation_anchor;
    s16 delay;
    s16 starts[16];
} Image168264Config;

typedef struct {
    Image168264Config *G32 config;
    SVECTOR origins[16];
    SVECTOR particles[64];
    s32 ray_scales[64];
    u8 phase;
    u8 unknown_385[3];
    u32 textures[2];
    s32 elapsed;
    u8 notified;
    u8 unknown_395[3];
    s32 tint_enabled;
} Image168264State;

extern VECTOR D_8013BFA0;
extern GsIMAGE D_8013BFB0[2];
extern Image168264Config D_8013BFE8[];

#endif

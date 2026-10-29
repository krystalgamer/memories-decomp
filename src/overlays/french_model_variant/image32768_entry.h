#ifndef FRENCH_MODEL_IMAGE32768_ENTRY_H
#define FRENCH_MODEL_IMAGE32768_ENTRY_H

#include "../../types.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 beam_r, beam_g, beam_b, flare_r, flare_g, flare_b;
    s16 plane_extent, plane_height, unknown_0A;
    s32 roll;
    s16 flare_size, tip_distance, fan_height, fan_count, fan_depth, unknown_1A;
    u8 beam_fade, unknown_1D, flare_fade, unknown_1F;
    s16 unknown_20[4], start, camera_end;
} Image32768Config;

typedef struct {
    Image32768Config *G32 config;
    SVECTOR tips[2];
    u16 phase, unknown_16;
    u32 elapsed;
    u16 ambient_phase, unknown_1E;
    s32 ambient;
    s32 textures[3];
    u8 r[3], g[3], b[3];
    u8 flare_r, flare_g, flare_b, flash, white, fan_r, fan_g, fan_b;
} Image32768State;

extern VECTOR D_8013C2DC;
extern GsIMAGE D_8013C2EC[2];
extern Image32768Config D_8013C324[];

#endif

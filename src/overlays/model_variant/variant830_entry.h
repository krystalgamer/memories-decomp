#ifndef MEMORIES_DECOMP_MODEL_VARIANT830_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT830_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/screen_projection.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x3C-byte command row: the ring colour, the spark size and spread,
 * the glow size, the radii and heights of the two ring sets, the debris size
 * and speed, the fade step, the start delay, the time the slot flashes white,
 * the camera cue times and their arguments, and the tint ramp. */
typedef struct {
    u8 r, g, b;
    u8 pad_03;
    u16 spark_size;
    u16 spark_spread;
    u16 glow_width;
    u16 glow_height;
    u16 inner_radius[3];
    u16 inner_depth;
    u16 outer_radius[3];
    u16 outer_depth;
    u16 debris_size;
    u16 debris_speed;
    u16 unknown_20;
    u8 fade_step;
    u8 pad_23;
    u16 unknown_24;
    u16 delay;
    u16 flash_start;
    u16 first_cue;
    u16 second_cue;
    s16 first_cue_arg;
    s16 second_cue_arg;
    s16 final_cue_arg;
    ModelTintColor tint_start;
    ModelTintColor tint_end;
} Model830Config;

typedef struct {
    u8 r, g, b;
} Model830Color;

/* The work area: the command row, the opponent position, the sparks, the
 * two ring sets (three edges per segment), the debris and its velocities,
 * the timers, the spark frames, the phase, the live spark count, the ambient
 * level, the textures, and the glow, ring and slot tint colours. */
typedef struct {
    Model830Config *G32 config;
    SVECTOR pos;
    SVECTOR sparks[64];
    SVECTOR inner[16][3];
    SVECTOR outer[16][3];
    SVECTOR debris[64];
    SVECTOR debris_velocities[64];
    u32 elapsed;
    u32 tick;
    u32 ring_scale;
    u16 spark_frame[64];
    u16 phase;
    u16 count;
    s32 level;
    s32 texture[4];
    Model830Color glow;
    Model830Color ring;
    Model830Color tint;
} Model830State;

extern GsIMAGE D_8013DC08[];
extern Model830Config D_8013DC78[];

s32 func_8013B004(u8 *context, s32 command);

#endif

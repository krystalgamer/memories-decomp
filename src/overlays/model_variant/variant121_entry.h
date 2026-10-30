#ifndef MEMORIES_DECOMP_MODEL_VARIANT121_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT121_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_effect_requests.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x20-byte command row: the box and line, trail and flash colours, the
 * glow colour and camera part, the particle spread, the trail and glow quad
 * half sizes, the slot animation window, the burst and trail durations, and
 * the delay and start times. */
typedef struct {
    u8 box_red, box_green, box_blue, trail_red, trail_green, trail_blue;
    u8 flash_red, flash_green, flash_blue, glow_red, glow_green, glow_blue;
    u8 part, unknown_0D;
    s16 spread;
    u16 trail_half_size;
    u16 glow_half_size;
    u16 anim_span;
    s16 duration, trail_duration, anim_start, delay, start;
} Model121Config;

/* The work area: the command row, 128 random particle offsets, the last three
 * positions of the camera part, the camera step counter, the two texture
 * words, the elapsed time and the tint flag. */
typedef struct {
    Model121Config *G32 config;
    /* Particles 0-127, then the three trail positions 128-130; one array
     * because the trail shift deliberately pulls the last particle into the
     * trail. */
    SVECTOR points[131];
    u8 camera_steps, unknown_41D[3];
    s32 texture[2];
    s32 elapsed;
    u8 started, unknown_42D[3];
    s32 tint;
} Model121State;

extern const VECTOR D_8013C840;
extern GsIMAGE D_8013C850[];
extern Model121Config D_8013C888[];

s32 func_8013B004(u8 *context, s32 command);

#endif

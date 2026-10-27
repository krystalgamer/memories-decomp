#ifndef MEMORIES_DECOMP_MODEL_VARIANT142_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT142_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2E-byte command row: the moving quad, burst quad and triangle
 * colours, the slot part the quads start on and the two parts the triangle
 * fans turn on, the moving quad half size, the target spread, the burst quad
 * half size, the burst lift and spread, the inner and outer fan radii, the
 * quad count and spacing, the move and fade durations, the burst, ambient and
 * fan durations, and the move, ambient and fan delays. */
typedef struct {
    u8 red, green, blue, burst_red, burst_green, burst_blue, fan_red, fan_green, fan_blue;
    u8 part, fan_parts[2];
    s16 half_size, width, burst_half_size, burst_lift, burst_spread, inner_radius, outer_radius;
    s16 count, spacing, duration, fade, burst_duration, ambient_duration, fan_duration;
    s16 move_delay, ambient_delay, fan_delay;
} Model142Config;

/* The 0x4A8-byte work area: the command row, up to 64 moving quad positions
 * and their targets around the other slot, the four burst offsets, the fan's
 * inner and outer points and centre, nine textures, the completion flag and
 * the elapsed time. */
typedef struct {
    Model142Config *G32 config;
    SVECTOR points[64];
    SVECTOR targets[64];
    SVECTOR offsets[4];
    SVECTOR inner[4];
    SVECTOR outer[6];
    SVECTOR center;
    u32 texture[9];
    u8 completed, unknown_4A1[3];
    s32 elapsed;
} Model142State;

extern GsIMAGE D_8013C3B0[];
extern Model142Config D_8013C4AC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

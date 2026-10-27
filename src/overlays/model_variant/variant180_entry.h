#ifndef MEMORIES_DECOMP_MODEL_VARIANT180_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT180_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x26-byte command row: the ray and ring colours, the ray half height and
 * half width, the ray spread, the ring radius and width, the ray, ring and
 * flash durations, the span and spacing of the rings, the spacing between rays
 * and the ring, ray and flash start times. */
typedef struct {
    u8 ray_r, ray_g, ray_b, ring_r, ring_g, ring_b;
    u8 unknown_06[2];
    s16 ray_height, ray_half_width, spread, ring_radius, ring_width, unknown_12;
    s16 ray_duration, ring_span, ring_duration, flash_duration;
    s16 ray_spacing, ring_spacing, ring_start, ray_start, flash_start;
} Model180Config;

/* The 0x334-byte work area: the command row, 32 ray points, the other slot's
 * position, a 66-point double ring, this slot's position, two texture words,
 * the elapsed time and the completion flag. */
typedef struct {
    Model180Config *G32 config;
    SVECTOR rays[32];
    SVECTOR opponent;
    SVECTOR ring[66];
    SVECTOR origin;
    s32 texture[2];
    s32 elapsed;
    u8 completed, unknown_331[3];
} Model180State;

extern GsIMAGE D_8013BC5C[];
extern Model180Config D_8013BC94[];

s32 func_8013B004(u8 *context, s32 command);

#endif

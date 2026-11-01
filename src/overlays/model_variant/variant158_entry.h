#ifndef MEMORIES_DECOMP_MODEL_VARIANT158_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT158_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x1C-byte command row, picked by command % 100: the particle and burst
 * colours, the slot part the particles leave from, the particle count, the
 * particle half size, the target spread, the lift of the travel arc, the
 * burst half size, the travel, fade and burst durations, the spacing between
 * particles and the start delay. */
typedef struct {
    u8 red, green, blue, burst_red, burst_green, burst_blue, part, count;
    s16 half_size, spread, lift, burst_half_size, travel_duration, fade_duration, burst_duration, interval, delay;
    u8 unknown_1A[2];
} Model158Config;

/* The 0x618-byte work area: the command row, 64 particle positions, 64
 * targets around the other slot and 64 burst positions, two texture words,
 * the completion flag, the elapsed time and the frame count. */
typedef struct {
    Model158Config *G32 config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    SVECTOR bursts[64];
    u32 texture[2];
    u8 completed, unknown_60D[3];
    s32 elapsed, updates;
} Model158State;

extern GsIMAGE gModelVariant158_D113C[];
extern Model158Config gModelVariant158_D1174[];

s32 ModelVariant158_HandleRequest(u8 *context, s32 command);

#endif

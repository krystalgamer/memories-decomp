#ifndef MEMORIES_DECOMP_MODEL_VARIANT96_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT96_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x26-byte command row: the flare, trail-tail, fan and burst colours,
 * the slot-data joint the flare starts on, the flare half size and swing,
 * the fan's ring radius and jitter, the burst half size, the flare, flight,
 * trail fade, fan and burst durations, the start delay and the flight
 * offset. */
typedef struct {
    u8 flare_red, flare_green, flare_blue;
    u8 tail_red, tail_green, tail_blue;
    u8 fan_red, fan_green, fan_blue;
    u8 burst_red, burst_green, burst_blue;
    u8 joint, unknown_0D;
    s16 flare_size, swing, ring_radius, ring_jitter, burst_size;
    s16 flare_duration, flight_duration, fade_duration, fan_duration, burst_duration;
    s16 delay, flight_offset;
} Model96Config;

/* The 0x154-byte work area: the command row, six trail positions, the fan's
 * inner and jagged outer rings (also seen as one flat run of 34 points,
 * which the setup fills in a single walk), the trail head, two textures and
 * the elapsed time. */
typedef struct {
    Model96Config *G32 config;
    SVECTOR trail[6];
    union {
        struct {
            SVECTOR ring[17];
            SVECTOR outer[17];
        } fan;
        SVECTOR points[34];
    } rings;
    u16 head, unknown_146;
    s32 texture[2];
    s32 elapsed;
} Model96State;

extern GsIMAGE D_8013C9DC[];
extern Model96Config D_8013CA14[];

s32 func_8013B004(u8 *context, s32 command);

#endif

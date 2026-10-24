#ifndef MEMORIES_DECOMP_MODEL_VARIANT752_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT752_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x44-byte command row: the burst colour, the sparkle speed, size, count
 * and rate, the ring radii and depth, the star size, the debris and ember
 * sizes, counts and speeds, the tint colours, mode, duration and period, the
 * timing of the camera presets and of the burst, and the camera mode. */
typedef struct {
    u8 r, g, b, pad_3;
    u16 sparkle_speed;
    u16 sparkle_size;
    u16 sparkle_max;
    u16 sparkle_rate;
    u16 inner_radius, outer_radius, ring_depth;
    u16 star_size;
    s16 debris_size;
    u16 debris_count;
    u16 debris_speed;
    u16 ember_count;
    s16 ember_size;
    u16 ember_speed;
    ModelTintColor tint_start, tint_end;
    u16 tint_mode, pad_2A;
    s32 tint_duration;
    u16 tint_period;
    u16 delay;
    u16 tint_stop;
    u16 burst_time;
    u16 camera_times[4];
    u16 mode, pad_42;
} Model752Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model752Color;

/* The work area: the command row, the sparkle positions, the two ring edges,
 * the debris and embers and their velocities, the debris frame, the sparkle
 * scales, the timers, the ring scale, the ambient level, the textures, the
 * sparkle colours and the burst colour. */
typedef struct {
    Model752Config *G32 config;
    SVECTOR sparkles[64];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR debris[8];
    SVECTOR debris_velocities[8];
    SVECTOR embers[64];
    SVECTOR ember_velocities[64];
    u16 debris_frame;
    u16 scales[64];
    u16 elapsed;
    u16 tick;
    u16 phase;
    u16 sparkle_count, unknown_80E;
    u32 ring_scale;
    s32 level;
    s32 texture[3];
    u16 burst_tpage, burst_clut;
    CVECTOR colors[64];
    Model752Color color;
} Model752State;

extern GsIMAGE D_8013C990[];
extern Model752Config D_8013C9E4[];

s32 func_8013B004(u8 *context, s32 command);

#endif

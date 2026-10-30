#ifndef MEMORIES_DECOMP_MODEL_VARIANT760_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT760_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x2C-byte command row: the ring colour, the slot model, the ring
 * radius, the height and tilt of the ring, the shard count and spread, the
 * flash half size, the spark half width and depth, the spark count, the spark
 * rise and scatter, the start delay, the spark start, the burst time and the
 * end of the effect. */
typedef struct {
    u8 r, g, b;
    u8 model;
    s16 radius;
    s16 height;
    s16 tilt;
    s16 unknown_0A;
    s16 shard_count;
    s16 unknown_0E;
    s32 shard_spread;
    s16 flash_half;
    s16 spark_half_width;
    s16 spark_half_depth;
    s16 spark_count;
    s16 spark_rise;
    s16 spark_scatter;
    s16 delay;
    s16 spark_start;
    s16 burst_time;
    s16 unknown_26;
    s16 unknown_28;
    s16 end;
} Model760Config;

/* The 0x32C-byte work area: the command row, the ring rim, the shards and
 * their spins, the flash and spark quads, the spark origin, the textures,
 * the ambient level, the line scale, the fade flags and phase counters, the
 * spark frames and the spark alphas. */
typedef struct {
    Model760Config *G32 config;
    SVECTOR ring[16];
    SVECTOR shards[32];
    SVECTOR spins[32];
    SVECTOR flash[4];
    SVECTOR spark[4];
    SVECTOR origin;
    s32 texture[3];
    s32 ambient;
    s16 radius;
    s16 line_scale;
    s16 line_count;
    s16 spark_shown;
    s16 fade_phase;
    s16 phase;
    s16 red_rising;
    s16 green_rising;
    s16 unknown_2EC;
    s16 white_rising;
    s16 spark_done;
    s16 elapsed;
    s16 timer;
    s16 frames[16];
    u8 red;
    u8 green;
    u8 blue;
    u8 white;
    u8 flash_level;
    u8 alpha[16];
} Model760State;

extern GsIMAGE D_8013CF78[3];
extern Model760Config D_8013CFCC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

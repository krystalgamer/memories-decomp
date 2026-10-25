#ifndef MEMORIES_DECOMP_MODEL_VARIANT110_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT110_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x26-byte command row: the sprite, fan, spark and dot colours, the
 * sprite size, the fan's ring radius and jitter, the dot and spark radii and
 * sizes, the dot and spark counts, the four durations and the start delay. */
typedef struct {
    u8 sprite_red, sprite_green, sprite_blue, fan_red, fan_green, fan_blue;
    u8 spark_red, spark_green, spark_blue, dot_red, dot_green, dot_blue;
    s16 sprite_size, ring_radius, ring_jitter, dot_radius, spark_size, spark_radius;
    s16 dot_count, spark_count, sprite_duration, fan_duration, spark_duration, dot_duration, delay;
} Model110Config;

/* The 0x720-byte work area: the command row, the fan's inner and jagged outer
 * rings, 128 dot offsets and 64 spark offsets, the texture, the elapsed time
 * and the completion flag. */
typedef struct {
    Model110Config *G32 config;
    SVECTOR ring[17];
    SVECTOR outer[17];
    SVECTOR dots[128];
    SVECTOR sparks[64];
    u32 texture[1];
    s32 elapsed;
    u8 unknown_71C[2], completed, unknown_71F;
} Model110State;

extern GsIMAGE D_8013C928[];
extern Model110Config D_8013C944[];

s32 func_8013B004(u8 *context, s32 command);

#endif

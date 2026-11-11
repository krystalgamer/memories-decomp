#ifndef MEMORIES_DECOMP_MODEL_VARIANT822_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT822_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x2A-byte command row: the burst colour and the five model parts the
 * projectiles leave from, the projectile size and travel time, the spark
 * and debris sizes and falls, the scatter speed, the debris size range, the
 * colour fade step, the delay, the camera switch time and the two times at
 * which the projectiles double in size. */
typedef struct {
    u8 r, g, b;
    u8 parts[5];
    u16 width;
    u16 height;
    u16 travel;
    u16 spark_width;
    u16 spark_height;
    u16 debris_size;
    u16 debris_fall;
    u16 speed;
    u16 shard_size;
    u16 shard_fall;
    u8 fade;
    u8 pad_1D;
    u16 unknown_1E;
    u16 unknown_20;
    u16 delay;
    u16 camera_time;
    u16 grow_time;
    u16 grow_again_time;
} Model822Config;

/* The work area: the command row, the five projectile positions and
 * velocities, the burst centres, the debris and shard offsets, the current
 * projectile size, the target slot position, the elapsed time, the
 * projectile spin and frame, the spark and debris frames, the ambient phase,
 * the stage, the growth step, the burst count, the ambient level, the
 * textures, the projectile colour and the burst colours. */
typedef struct {
    Model822Config *config;
    SVECTOR position[5];
    SVECTOR velocity[5];
    SVECTOR centers[32];
    SVECTOR debris[32][3];
    SVECTOR shards[32][8];
    s16 width;
    s16 height;
    SVECTOR origin;
    u32 elapsed;
    s16 spin;
    u16 frame;
    u16 spark_frames[32];
    u16 debris_frames[32];
    u16 phase;
    u16 stage;
    u16 grow;
    u16 count;
    s32 level;
    s32 texture[3];
    u8 red, green, blue;
    u8 r[32];
    u8 g[32];
    u8 b[32];
} Model822State;

extern GsIMAGE gModelVariant822_aImages[];
extern Model822Config gModelVariant822_aConfigs[];

s32 ModelVariant822_HandleRequest(u8 *context, s32 command);

#endif

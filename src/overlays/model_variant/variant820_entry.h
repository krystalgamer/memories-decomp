#ifndef MEMORIES_DECOMP_MODEL_VARIANT820_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT820_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x22-byte command row: the burst colour and the model part the
 * projectile leaves from, the projectile size and travel time, the spark
 * and debris sizes and falls, the scatter speed, the debris size range, the
 * colour fade step, the delay and the camera switch time. */
typedef struct {
    u8 r, g, b, part;
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
    u8 pad_19;
    u16 unknown_1A;
    u16 unknown_1C;
    u16 delay;
    u16 camera_time;
} Model820Config;

/* The work area: the command row, the projectile position and velocity, the
 * burst centres, the debris and shard offsets, the target slot position, the
 * elapsed time, the projectile spin and frame, the spark and debris frames,
 * the ambient phase, the stage, the burst count, the ambient level, the
 * textures, the projectile colour and the burst colours. */
typedef struct {
    Model820Config *G32 config;
    SVECTOR position;
    SVECTOR velocity;
    SVECTOR centers[32];
    SVECTOR debris[32][3];
    SVECTOR shards[32][8];
    SVECTOR origin;
    u32 elapsed;
    s16 spin;
    u16 frame;
    u16 spark_frames[32];
    u16 debris_frames[32];
    u16 phase;
    u16 stage;
    u16 count;
    s32 level;
    s32 texture[3];
    u8 red, green, blue;
    u8 r[32];
    u8 g[32];
    u8 b[32];
} Model820State;

extern GsIMAGE gModelVariant820_D1B10[];
extern Model820Config gModelVariant820_D1B64[];

s32 ModelVariant820_HandleRequest(u8 *context, s32 command);

#endif

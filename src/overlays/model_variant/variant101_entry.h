#ifndef MEMORIES_DECOMP_MODEL_VARIANT101_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT101_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x48-byte command row: the ring, flash and spark colours, the ring
 * width, flash size, spark spread, joint, per-joint ring and burst counts,
 * the flash, ring fade, burst, start delay and ring end times, the ring
 * joints and the per-burst start times. */
typedef struct {
    u8 ring_red, ring_green, ring_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 unknown_09;
    s16 width;
    s16 size;
    s16 spread;
    s16 joint_count;
    s16 ring_count;
    s16 burst_count;
    s16 flash;
    s16 ring_fade;
    s16 burst;
    s16 delay;
    s16 end;
    u8 joints[8];
    s16 start[16];
} Model101Config;

/* The 0x65C-byte work area: the command row, the ring trail points of every
 * joint, the spark offsets, each burst's rotation, the ring trail index, the
 * three textures, the completion flag and the elapsed time. */
typedef struct {
    Model101Config *G32 config;
    SVECTOR points[128];
    SVECTOR sparks[64];
    SVECTOR rotations[8];
    u8 index;
    u8 unknown_645[3];
    u32 texture[3];
    u8 completed;
    u8 unknown_655[3];
    s32 elapsed;
} Model101State;

extern GsIMAGE D_8013C2C8[];
extern Model101Config D_8013C31C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

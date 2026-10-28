#ifndef MEMORIES_DECOMP_MODEL_VARIANT829_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT829_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x24-byte command row: the streak head colour, the spark colour, the
 * model part the streaks leave from, the streak spread, the sprite size, the
 * spark fall step, the delay, the end of emission, and the timing and offsets
 * of the camera presets. */
typedef struct {
    u8 r, g, b;
    u8 spark_r, spark_g, spark_b;
    u8 part, pad_7;
    u16 spread;
    u16 pad_a;
    s16 size;
    s16 fall;
    u16 delay;
    u16 emit_end;
    u16 camera_times[2];
    s16 camera_args[3];
    u16 pad_1e;
    u16 pad_20[2];
} Model829Config;

/* A streak: the tail and the head that flies towards the other slot. */
typedef struct {
    SVECTOR tail;
    SVECTOR head;
} Model829Streak;

/* The work area: the command row, the streaks, their shared step, the sparks,
 * the elapsed time, the spark and burst frames, the phase, the per-streak
 * arrival flags and step counts, the streak and spark counts, the camera
 * sequence, the ambient level and the texture. */
typedef struct {
    Model829Config *G32 config;
    Model829Streak streaks[256];
    SVECTOR step;
    SVECTOR sparks[64];
    u32 elapsed;
    u16 spark_frames[64];
    u16 frames[256];
    u16 phase;
    u16 arrived[256];
    u16 moves[256];
    u16 streak_count;
    u16 spark_count;
    u16 camera;
    s32 level;
    s32 texture[1];
} Model829State;

extern GsIMAGE D_8013C150[];
extern Model829Config D_8013C16C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

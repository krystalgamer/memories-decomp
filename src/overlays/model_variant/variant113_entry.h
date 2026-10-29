#ifndef MEMORIES_DECOMP_MODEL_VARIANT113_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT113_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1E-byte command row, picked directly by the command: the ring and
 * spark colours, the ring, burst and spark half sizes, the vertical span the
 * rings scroll through, the radius of the spark targets, the delay between
 * rings appearing, the fade and total durations, the spark flight time, the
 * spark count and the start delay. */
typedef struct {
    u8 red, green, blue, spark_red, spark_green, spark_blue, unknown_06[2];
    s16 ring_half_size, ring_span, burst_half_size, spark_half_size, radius;
    s16 interval, fade_duration, duration, spark_duration, count, delay;
} Model113Config;

/* The 0x440-byte work area: the command row, four ring heights, 128 spark
 * targets, the other slot's position, two texture words, the elapsed time,
 * the completion flag and the frame count. */
typedef struct {
    Model113Config *G32 config;
    SVECTOR rings[4];
    SVECTOR targets[128];
    SVECTOR center;
    s32 texture[2];
    s32 elapsed;
    u8 completed, unknown_439[3];
    s32 updates;
} Model113State;

extern GsIMAGE D_8013C384[];
extern Model113Config D_8013C3BC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT98_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT98_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/gpu_packets.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x2E-byte command row: the fan, grid, spark and ring colours, the
 * joint the fan follows, the fan depth and radius, the grid size, the spark
 * width and rise, the ring size, the fan grow and hold times, the grid
 * grow time, the spark and ring lifetimes, the fan blade spread, the spark
 * and ring spacing, the start delay and the grid start time. */
typedef struct {
    u8 fan_color[3];
    u8 grid_color[3];
    u8 spark_color[3];
    u8 ring_color[3];
    u8 part;
    u8 unknown_0D;
    s16 depth;
    s16 radius;
    s16 grid_size;
    s16 spark_width;
    s16 rise;
    s16 ring_size;
    s16 grow;
    s16 hold;
    s16 grid_grow;
    s16 spark_life;
    s16 ring_life;
    s16 spread;
    s16 spark_spacing;
    s16 ring_spacing;
    s16 delay;
    s16 grid_start;
} Model98Config;

/* The work area: the command row, the fan centre and its six
 * blade edges, a 9x9 grid, 144 sparks, 16 rings, the opponent position,
 * six texture words, the blink toggle, the elapsed time and the
 * completion flag. */
typedef struct {
    Model98Config *G32 config;
    SVECTOR center;
    SVECTOR blades[12];
    SVECTOR grid[81];
    SVECTOR sparks[144];
    SVECTOR rings[16];
    SVECTOR target;
    s32 texture[6];
    u16 toggle;
    u8 unknown_816[2];
    s32 elapsed;
    u8 completed;
    u8 unknown_81D[3];
} Model98State;

extern GsIMAGE D_8013C5E4[];
extern Model98Config D_8013C68C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

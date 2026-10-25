#ifndef MEMORIES_DECOMP_MODEL_VARIANT72_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT72_ENTRY_H

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

/* One 0x32-byte command row: the ribbon's start and end colours, the sprite
 * and fan colours, the ribbon grid's columns and rows, the sprite size, the
 * fan's ring radius and jitter, the sprite and fan durations, the lead time,
 * the frames that start and end the move, the eight parts the ribbon columns
 * follow, and the sparks' colour, size, spread, lift and duration. */
typedef struct {
    u8 start_red, start_green, start_blue, end_red, end_green, end_blue;
    u8 sprite_red, sprite_green, sprite_blue, fan_red, fan_green, fan_blue;
    u8 columns, rows;
    s16 sprite_size, ring_radius, ring_jitter, sprite_duration, fan_duration, lead, start, end;
    u8 parts[8];
    u8 spark_red, spark_green, spark_blue, unknown_29;
    s16 spark_size, spark_spread, spark_lift, spark_duration;
} Model72Config;

/* One ribbon row's colour. */
typedef struct {
    u8 red, green, blue, unknown_03;
} Model72Colour;

/* The 0x904-byte work area: the elapsed time, the ribbon's grid of part
 * positions (one row recorded a frame), the fan's inner and jagged outer
 * rings, 48 spark offsets, one colour per ribbon row, the next row to record,
 * the texture, the completion flag and the spark switch. */
typedef struct {
    s32 elapsed;
    SVECTOR points[192];
    SVECTOR ring[17];
    SVECTOR outer[17];
    SVECTOR sparks[48];
    Model72Colour colours[24];
    s16 row, unknown_8F6;
    u32 texture[1];
    u8 completed, sparks_on, unknown_8FE[2];
    Model72Config *G32 config;
} Model72State;

extern GsIMAGE D_8013CD78[];
extern Model72Config D_8013CD94[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT174_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT174_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/libgte.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x28-byte command row, picked by command % 100: the ribbon, grid, flash
 * and spark colours, the slot part, the ribbon count, the sparks per ribbon,
 * the ribbon half width, height and depth, the grid half size, the spark half
 * size and spread, the travel, fade, flash and spark durations, the spacing
 * and the delay. */
typedef struct {
    u8 red, green, blue;
    u8 grid_red, grid_green, grid_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 part, count, sparks, unknown_0f;
    s16 half_width, height, depth, grid_half_size, spark_half_size, spark_spread;
    s16 travel_duration, fade_duration, flash_duration, spark_duration, spacing, delay;
} Model174Config;

/* The 0x9B8-byte work area: the command row, the 34-point ribbon (two rows of
 * 17), 64 ribbon positions and their origin, a block this entry does not use,
 * the nine grid points, up to 128 spark offsets, the sixteen grid face
 * pointers, four texture words, the completion flag, the elapsed time and a
 * frame counter. */
typedef struct {
    Model174Config *G32 config;
    SVECTOR ribbon[34];
    SVECTOR positions[64];
    SVECTOR origin;
    u8 unknown_31C[0x1F8];
    SVECTOR vertices[9];
    SVECTOR spark_points[128];
    SVECTOR *G32 faces[16];
    u32 texture[4];
    u8 completed;
    u8 unknown_9AD[3];
    s32 elapsed, animation;
} Model174State;

extern GsIMAGE gModelVariant174_D17CC[];
extern Model174Config gModelVariant174_D183C[];

s32 ModelVariant174_HandleRequest(u8 *context, s32 command);

#endif

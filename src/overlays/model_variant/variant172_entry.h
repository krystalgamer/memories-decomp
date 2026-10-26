#ifndef MEMORIES_DECOMP_MODEL_VARIANT172_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT172_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x1A-byte command row: the dot and quad colours, the slot part the dots
 * start from, the start jitter, the target sphere's radius, the quad half
 * size, the gather and fly durations, the spacing between groups, the point
 * total, the group size and the start delay. */
typedef struct {
    u8 red, green, blue, quad_red, quad_green, quad_blue, part, unknown_07;
    s16 jitter, radius, half_size, gather_duration, fly_duration, spacing, total, group, delay;
} Model172Config;

/* The 0x2014-byte work area: the command row, 512 dot positions and 512 target
 * points, the texture, the completion flag, the elapsed time and a frame
 * count. */
typedef struct {
    Model172Config *G32 config;
    SVECTOR positions[512];
    SVECTOR targets[512];
    u32 texture[1];
    u8 completed, unknown_2009[3];
    s32 elapsed;
    s32 frames;
} Model172State;

extern GsIMAGE D_8013BD18[];
extern Model172Config D_8013BD34[];

s32 func_8013B004(u8 *context, s32 command);

#endif

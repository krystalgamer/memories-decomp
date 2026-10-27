#ifndef MEMORIES_DECOMP_MODEL_VARIANT171_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT171_ENTRY_H

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

/* One 0x1A-byte command row: the gathering and the burst quad colours, the
 * slot part the quads gather on, the gathering quad half size, the start
 * sphere's radius, the burst quad half size, the gather and burst durations,
 * the spacing between groups, the quad total, the group size and the start
 * delay. */
typedef struct {
    u8 red, green, blue, burst_red, burst_green, burst_blue, part, unknown_07;
    s16 half_size, radius, burst_half_size, gather_duration, burst_duration, spacing, total, group, delay;
} Model171Config;

/* The 0x2048-byte work area: the command row, 512 moving quad positions and
 * the 512 start points they burst at, the fourteen textures, the completion
 * flag, the elapsed time and a frame count. */
typedef struct {
    Model171Config *G32 config;
    SVECTOR positions[512];
    SVECTOR starts[512];
    u32 texture[14];
    u8 completed, unknown_203D[3];
    s32 elapsed;
    s32 frames;
} Model171State;

extern GsIMAGE D_8013BF54[];
extern Model171Config D_8013C0DC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

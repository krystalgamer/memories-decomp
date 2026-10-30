#ifndef MEMORIES_DECOMP_MODEL_VARIANT157_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT157_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"
#include "../../psyq/sdk_internal.h"

/* One 0x1C-byte command row, picked by command % 100: the head, trail and
 * burst colours, the joint the strike starts from, the number of repeats, the
 * quad half width, the arc radius, the burst grid spacing, the flight time,
 * an unread halfword, the burst time and the start delay. */
typedef struct {
    u8 head_r, head_g, head_b;
    u8 trail_r, trail_g, trail_b;
    u8 burst_r, burst_g, burst_b;
    u8 part, repeats, unknown_0b;
    s16 width, radius, spacing, duration, unknown_14, burst_duration, delay;
    s16 unknown_1a;
} Model157UsConfig;

/* The 0x10B8-byte work area: the command row, 256 flight path offsets, the
 * 3x3 burst grid and the four quads built from it, 256 flight rotations,
 * seven texture words, the burst-announced flag, the elapsed time, the
 * repeat count and the frame count. */
typedef struct {
    Model157UsConfig *G32 config;
    SVECTOR path[256];
    SVECTOR grid[9];
    SVECTOR *G32 quads[4][4];
    SVECTOR spin[256];
    u32 texture[7];
    u8 announced, unknown_10a9;
    s16 unknown_10aa;
    s32 elapsed;
    s16 repeat, unknown_10b2;
    s32 frames;
} Model157UsState;

extern GsIMAGE D_8013C47C[];
extern Model157UsConfig D_8013C540[];

s32 func_8013B004(u8 *context, s32 command);

#endif

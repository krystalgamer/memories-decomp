#ifndef MEMORIES_DECOMP_MODEL_VARIANT153US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT153US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x1E-byte command row: the sheet colour, the slot part it starts
 * from, the sheet half-width, the height offset towards the opponent, the
 * orbit radius, the grow, travel, spin and hold times, the closing flash
 * time, the per-frame spin and the start delay. */
typedef struct {
    u8 red, green, blue, part;
    s16 half_size, height, radius;
    s16 grow_duration, travel_duration, spin_duration, hold_duration;
    s16 flash_duration;
    s16 spin[3];
    s16 unknown_1A;
    s16 delay;
} Model153UsConfig;

/* The 0x24C-byte work area: the command row, the two anchor points, the
 * 5x5 sheet grid, a five-entry trail per anchor, the grid corners of each
 * of the 4x4 quads, the sheet rotation and its spin, the texture, the
 * completion flag, the trail cursor, the elapsed time and the frame count.
 * The grid, trails and corners are flat because the code walks them as one
 * run: grid point (row, column) is grid[row * 5 + column], trail k entry x
 * is trail[k * 5 + x], and quad (row, column) owns corners[(row * 4 +
 * column) * 4] onwards (top-left, top-right, bottom-left, bottom-right). */
typedef struct {
    Model153UsConfig *G32 config;
    SVECTOR anchor[2];
    SVECTOR grid[25];
    SVECTOR trail[10];
    SVECTOR *G32 corners[64];
    SVECTOR rotation;
    s16 spin[3];
    u8 unknown_23A[2];
    u32 texture[1];
    u8 completed;
    u8 trail_index;
    s32 elapsed;
    s32 frames;
} Model153UsState;

extern GsIMAGE D_8013C45C[];
extern Model153UsConfig D_8013C478[];

s32 func_8013B004(u8 *context, s32 command);

#endif

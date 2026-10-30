#ifndef MEMORIES_DECOMP_MODEL_VARIANT178_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT178_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/graphics_constants.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2A-byte command row, picked by command % 100: the line, box, strip
 * and band colours, the grid size, the box height, the strip sway, the base
 * band speed, the fade and box durations, the light ramp, the box interval,
 * the boxes per row, the box rows and the line, box and band start times. */
typedef struct {
    u8 line_color[3];
    u8 box_color[3];
    u8 strip_color[3];
    u8 band_color[3];
    s16 unknown_C;
    s16 size;
    s16 height;
    s16 sway;
    s16 speed;
    s16 fade;
    s16 box_time;
    s16 ramp;
    s16 unknown_1C;
    s16 box_interval;
    s16 box_count;
    s16 box_rows;
    s16 start;
    s16 box_start;
    s16 band_start;
} Model178Config;

/* The 0x988-byte work area: the command row, a 52-point square of grid
 * lines, the other slot's position, the random box picks, the band phases
 * and speeds, the band speed offset and scroll, three texture words, the
 * completion flag, the elapsed time and the frame count. */
typedef struct {
    Model178Config *G32 config;
    SVECTOR grid[52];
    SVECTOR target;
    s16 picks[512];
    s16 phase[GRAPHICS_DEFAULT_HEIGHT];
    s16 speed[GRAPHICS_DEFAULT_HEIGHT];
    s16 offset;
    s16 scroll;
    u32 texture[3];
    u8 completed, unknown_97D[3];
    s32 elapsed;
    s32 frames;
} Model178State;

extern GsIMAGE D_8013CB0C[];
extern Model178Config D_8013CB60[];

s32 func_8013B004(u8 *context, s32 command);

#endif

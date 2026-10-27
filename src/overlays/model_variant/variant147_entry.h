#ifndef MEMORIES_DECOMP_MODEL_VARIANT147_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT147_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x3C-byte command row: the tube colour, the low and high grid colour
 * bounds, the spark and two beam colours, the tube width, the wave height,
 * sway and drift, the scroll speed, the grid size, the tube height, the grid
 * ripple, the beam width, the path height and radius, the wave period, the
 * fade, beam and flash durations and the tube, fade-out, grid and beam start
 * times. */
typedef struct {
    u8 color[3];
    u8 low[3], high[3];
    u8 spark_color[3];
    u8 beam_color[3], beam_color2[3];
    s16 width, wave_height, sway, drift, speed, grid_size, height, ripple;
    s16 beam_width, path_height, path_radius, period, unknown_2A;
    s16 fade, beam_time, flash_time, unknown_32;
    s16 tube_start, fade_out, grid_start, beam_start;
} Model147Config;

/* The 0x1350-byte work area: the command row, a 13x13 grid with its
 * colours, the tube's near and far rings, 34 spokes, 32 sparks, a 256-point
 * spiral path, the spoke animation phases, the tube's depth offset, five
 * texture words, the completion flag and the elapsed time. */
typedef struct {
    Model147Config *G32 config;
    SVECTOR grid[169];
    CVECTOR colors[169];
    SVECTOR ring[17];
    SVECTOR far_ring[17];
    SVECTOR spokes[34];
    SVECTOR sparks[32];
    SVECTOR path[256];
    u8 phase[34];
    s16 offset;
    s32 texture[5];
    u8 completed, unknown_1349[3];
    s32 elapsed;
} Model147State;

extern GsIMAGE D_8013E550[];
extern Model147Config D_8013E5DC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

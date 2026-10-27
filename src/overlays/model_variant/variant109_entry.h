#ifndef MEMORIES_DECOMP_MODEL_VARIANT109_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT109_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x36-byte command row: the sprite, beam, square, grid-range and
 * flash colours, the attachment part, the sprite size, spread, beam radius,
 * square size, grid size, base height and wave amplitude, the flash width,
 * the sprite, rise and spin periods, the fade and flash times, the lead
 * time, the delay and the duration. */
typedef struct {
    u8 sprite_red, sprite_green, sprite_blue;
    u8 beam_red, beam_green, beam_blue;
    u8 square_red, square_green, square_blue;
    u8 low[3], high[3];
    u8 flash_red, flash_green, flash_blue;
    u8 part, unknown_13;
    s16 sprite_size, spread, beam_radius, square_size, unknown_1C;
    s16 grid_size, base, amplitude, flash_width;
    s16 sprite_period, rise_duration, spin_period, fade, flash_time;
    s16 lead, delay, duration;
} Model109Config;

/* One grid vertex colour. */
typedef struct {
    u8 red, green, blue, unknown_03;
} Model109Colour;

/* The 0x8E0-byte work area: the command row, eight sprite directions, the
 * nine-point beam ring, two four-vertex squares, the 13x13 wave grid and its
 * colours, the per-sprite timers, five texture words and the elapsed time. */
typedef struct {
    Model109Config *G32 config;
    SVECTOR directions[8];
    SVECTOR ring[9];
    SVECTOR squares[8];
    SVECTOR grid[169];
    Model109Colour colours[169];
    s16 timers[8];
    u32 texture[5];
    s32 elapsed;
} Model109State;

extern GsIMAGE D_8013E530[];
extern Model109Config D_8013E5BC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

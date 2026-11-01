#ifndef MEMORIES_DECOMP_MODEL_VARIANT138_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT138_ENTRY_H

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

/* One 0x28-byte command row: the wave's colour, two unused colour ranges,
 * the sprites' colour, then the sprite size, the bounce height, the sway and
 * drift of the ring, the spin speed, the grid size, the base height, the wave
 * amplitude, the period, the fade time, the lead time and the frame the fade
 * starts. */
typedef struct {
    u8 red, green, blue;
    u8 low[3], high[3];
    u8 sprite_red, sprite_green, sprite_blue;
    s16 sprite_size, bounce, sway, drift, speed, grid_size, base, amplitude, period, unknown_1E;
    s16 fade, unknown_22, lead, start;
} Model138Config;

/* One rainbow vertex colour. */
typedef struct {
    u8 red, green, blue, unknown_03;
} Model138Colour;

/* The 0xDAC-byte work area: the command row, the 13x13 wave grid and its
 * colours, the ring's 9x17 colours, the ring's lower and upper edges, 34
 * orbiting sprites, 32 rising sprites, the orbiting sprites' frames, the
 * spin angle, three texture words, the completion flag and the elapsed
 * time. */
typedef struct {
    Model138Config *G32 config;
    SVECTOR grid[169];
    Model138Colour colours[169];
    Model138Colour ring[153];
    SVECTOR lower[17];
    SVECTOR upper[17];
    SVECTOR orbit[34];
    SVECTOR rising[32];
    u8 frames[34];
    s16 spin;
    s32 texture[3];
    u8 completed, unknown_DA5[3];
    s32 elapsed;
} Model138State;

extern char gModelVariant138_D3314[];
extern GsIMAGE gModelVariant138_D3328[];
extern Model138Config gModelVariant138_D337C[];

s32 ModelVariant138_HandleRequest(u8 *context, s32 command);

#endif

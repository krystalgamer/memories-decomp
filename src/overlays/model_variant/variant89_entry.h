#ifndef MEMORIES_DECOMP_MODEL_VARIANT89_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT89_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x26-byte command row: the flash-fan, panel and shell colours, the
 * attachment part, the ring radius, the panel size, the two arc radii and
 * the flash, travel, fade-in, rise, lift, shell, gather and stagger times,
 * then the start delay. */
typedef struct {
    u8 flash_red, flash_green, flash_blue;
    u8 panel_red, panel_green, panel_blue;
    u8 shell_red, shell_green, shell_blue;
    u8 part, unknown_0A, unknown_0B;
    s16 ring_radius, panel_size, arc_radius, arc_outer;
    s16 flash_duration, travel, fade_in, rise, lift;
    s16 shell_duration, gather, stagger, delay;
} Model89Config;

/* The work area: the command row, fifteen flying panel positions, the
 * 18-point flash ring, the 4x6 panel grid, its centre, the 18-point shell
 * arc, two texture words, the completion flag and the elapsed time. */
typedef struct {
    Model89Config *G32 config;
    SVECTOR positions[15];
    SVECTOR ring[18];
    SVECTOR grid[24];
    SVECTOR center;
    SVECTOR arc[18];
    s32 texture[2];
    u8 completed;
    s32 elapsed;
} Model89State;

extern GsIMAGE D_8013D42C[];
extern Model89Config D_8013D464[];

s32 func_8013B004(u8 *context, s32 command);

#endif

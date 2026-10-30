#ifndef MEMORIES_DECOMP_MODEL_VARIANT137_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT137_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x36-byte command row, picked by command % 100: the spark, orb and
 * drop colours, the slot part, the spark height and spread, the ring radius
 * and width, the grid spacing, the orb lift and sway, the drop spread and
 * size, the spark, orb grow, hold and flight durations, the drop life, the
 * flash time, the orb spin, the drop spacing and the spark, orb and drop
 * start times. */
typedef struct {
    u8 red, green, blue;
    u8 unknown_03[3];
    u8 orb_red, orb_green, orb_blue;
    u8 drop_red, drop_green, drop_blue;
    u8 part, unknown_0D;
    s16 height, spread, ring_radius, ring_width, grid, lift, sway, drop_spread, drop_size;
    s16 spark, grow, hold, flight, drop, flash, spin, drop_spacing;
    s16 spark_start, orb_start, drop_start;
} Model137Config;

/* The 0x53C-byte work area: the command row, 64 spark offsets, a 66-point
 * ring, a 3x3 grid, the orb position, 16 drops, the four grid quads, the orb
 * spin angle, three textures, the completion flag and the elapsed time. */
typedef struct {
    Model137Config *G32 config;
    SVECTOR sparks[64];
    SVECTOR ring[66];
    SVECTOR grid[9];
    SVECTOR orb;
    SVECTOR drops[16];
    SVECTOR *G32 quads[4][4];
    s32 angle;
    u32 texture[3];
    u8 completed, unknown_535[3];
    s32 elapsed;
} Model137State;

extern GsIMAGE D_8013CC34[];
extern Model137Config D_8013CC88[];

s32 func_8013B004(u8 *context, s32 command);

#endif

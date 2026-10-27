#ifndef MEMORIES_DECOMP_MODEL_VARIANT789_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT789_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x3C-byte command row: the flare and glow colours, the three model
 * points the flares leave from in turn, the trail depth step and width, the
 * arc lift, the flare count, the flight steps, the spawn spread, the trail
 * length, the ray spread, width and count, the glow size, the two ring
 * radii, the two glow growth rates, the fade step and the six launch times. */
typedef struct {
    u8 r, g, b;
    u8 glow_r, glow_g, glow_b;
    u8 part[3];
    u8 unknown_09;
    u16 depth;
    u16 width;
    u16 lift;
    u16 count;
    u16 steps;
    u16 spread;
    u16 trail_count;
    u16 ray_spread;
    u16 ray_width;
    u16 ray_count;
    u16 glow_size;
    u16 ring_outer;
    u16 ring_inner;
    s32 grow;
    s32 slow_grow;
    u8 fade, unknown_2D;
    u16 time[6];
    u16 unknown_3A;
} Model789Config;

/* The work area: the command row, six flares with their position, launch
 * point, velocity, orientation and arc, the rays around each flare, the
 * sixteen-step glow ring, the flight frames, the launch count, the flare
 * states, the phase, the timer, the glow scales, the ambient level, the
 * three textures and the per-flare flare, glow and ray colours. */
typedef struct {
    Model789Config *G32 config;
    SVECTOR pos[6];
    SVECTOR start[6];
    SVECTOR velocity[6];
    SVECTOR angle[6];
    SVECTOR arc[6];
    SVECTOR rays[6][32];
    SVECTOR ring[16][2];
    u16 frame[6];
    u16 count;
    u16 state[6];
    u16 phase;
    u16 unknown_810[6];
    u32 elapsed;
    u32 scale[6];
    s32 ambient;
    s32 texture[3];
    u8 r[6];
    u8 g[6];
    u8 b[6];
    u8 glow_r[6];
    u8 glow_g[6];
    u8 glow_b[6];
    u8 ray[6];
} Model789State;

extern GsIMAGE D_8013CDCC[];
extern Model789Config D_8013CE20[];

s32 func_8013B004(u8 *context, s32 command);

#endif

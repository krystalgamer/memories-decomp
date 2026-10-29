#ifndef MEMORIES_DECOMP_MODEL_VARIANT837_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT837_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 12-byte command row: the effect colour, the model part the orb
 * gathers on, the radius of the gathering disc and the times at which the
 * camera turns, the orb is launched and the gathering stops. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 radius;
    u16 orbit_time;
    u16 launch_time;
    u16 stop_time;
} Model837Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model837Color;

/* The work area: the command row, the gathering disc rim, the 32 four-point
 * streaks and their angles, the orb position and step, the sparks and
 * glints with their velocities, the ring offsets, the falling rays, the
 * debris and its velocities, the timers, the phase, the glow scale, the
 * ring scales, the streak lengths, the streak, spark, glint and ring counts,
 * the ambient level, the fixed ring texture, the three textures, the glow,
 * halo, streak, core, spark, glint, burst and ring colours and the screen
 * flash level. */
typedef struct {
    Model837Config *G32 config;
    SVECTOR rim[32];
    SVECTOR streaks[32][4];
    SVECTOR streak_angle[32];
    SVECTOR orb;
    SVECTOR orb_step;
    SVECTOR sparks[32];
    SVECTOR spark_velocity[32];
    SVECTOR glints[16];
    SVECTOR glint_velocity[16];
    SVECTOR rings[32];
    SVECTOR rays[32];
    SVECTOR debris[64];
    SVECTOR debris_velocity[64];
    u32 elapsed;
    u32 frame;
    u16 phase;
    u16 glow_scale;
    u16 ring_scale[32];
    u16 streak_length[32];
    u16 streak_count;
    u16 spark_count;
    u16 glint_count;
    u16 ring_count;
    s32 ambient;
    u16 ring_tpage;
    u16 ring_clut;
    s32 texture[3];
    Model837Color glow_color;
    Model837Color halo_color;
    CVECTOR streak_color[32];
    CVECTOR core_color;
    CVECTOR spark_color[32];
    CVECTOR glint_color[16];
    Model837Color burst_color;
    CVECTOR ring_color[32];
    u8 flash;
} Model837State;

extern GsIMAGE D_8013DF8C[];
extern Model837Config D_8013DFE0[];

s32 func_8013B004(u8 *context, s32 command);

#endif

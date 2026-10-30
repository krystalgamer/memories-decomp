#ifndef MEMORIES_DECOMP_MODEL_VARIANT804_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT804_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x2A-byte command row: the bolt, glow and spark colours, the three
 * model points (the bolts' source, the aim point and the charge point),
 * the bolt depth and half height, the flight length, the bolt count, the
 * impact glow size, the spark spread, count and size, the ray width,
 * spread and count, the fade step and the two timings: the charge end and
 * the launch. */
typedef struct {
    u8 r, g, b;
    u8 glow_r, glow_g, glow_b;
    u8 spark_r, spark_g, spark_b;
    u8 part;
    u8 aim_part;
    u8 charge_part;
    u16 depth;
    u16 size;
    u16 steps;
    u16 count;
    u16 glow_size;
    u16 spark_spread;
    u16 spark_count;
    u16 spark_size;
    u16 ray_width;
    u16 ray_spread;
    u16 ray_count;
    u8 fade, unknown_23;
    u16 unknown_24;
    u16 charge_end;
    u16 launch;
} Model804Config;

/* The work area: the command row; per bolt its position, step, angle,
 * target, sparks and their velocities, rays and 32-point trail; the timer;
 * the phase, the arrival flags, trail heads and lengths (the phase and
 * first arrival flag also read as one word); the ambient
 * level, the textures, the per-bolt brightness and the per-bolt bolt,
 * glow and spark colours. */
typedef struct {
    Model804Config *G32 config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR target[8];
    SVECTOR spark[8][32];
    SVECTOR spark_step[8][32];
    SVECTOR rays[8][32];
    SVECTOR trail[8][32];
    u32 elapsed;
    union {
        struct {
            u16 phase;
            u16 arrived[8];
            u16 trail_head[8];
            u16 tail[8];
        } s;
        u32 phase_arrived; /* the phase and the first arrival flag as one word */
    } track;
    s32 ambient;
    s32 texture[3];
    u8 bright[8];
    u8 bolt_r[8];
    u8 bolt_g[8];
    u8 bolt_b[8];
    u8 glow_r[8];
    u8 glow_g[8];
    u8 glow_b[8];
    u8 spark_r[8];
    u8 spark_g[8];
    u8 spark_b[8];
} Model804State;

extern GsIMAGE D_8013D344[3];
extern Model804Config D_8013D398[];

s32 func_8013B004(u8 *context, s32 command);

#endif

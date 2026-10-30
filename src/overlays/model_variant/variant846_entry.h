#ifndef MEMORIES_DECOMP_MODEL_VARIANT846_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT846_ENTRY_H

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

/* One 0x3C-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, its rays and sparks,
 * the two fade steps, the two bolts' launch times and the CLUT mode of
 * the second and fourth textures. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part, unknown_0D;
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 ray_spread;
    u16 ray_width;
    u16 ray_count;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_33;
    u8 ray_fade, unknown_35;
    u16 start;
    u16 second_start;
    u16 mode;
} Model846Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, smoke puffs
 * and their velocities, impact rays, 64 drifting impact flecks and their
 * velocities, and sparks; the timer, the per-bolt flight counters, puff and
 * spark frames, trail lengths, spark counts and trail heads; the bolt count,
 * the arrival flags, the phase, the ambient level, the six textures and the
 * per-bolt bolt, glow and ray colours. The work area is 0x1126 bytes. */
typedef struct {
    Model846Config *G32 config;
    SVECTOR pos[2];
    SVECTOR step[2];
    SVECTOR angle[2];
    SVECTOR ring[2][32];
    SVECTOR target[2];
    SVECTOR trail[2][32];
    SVECTOR origin[2];
    SVECTOR puffs[2][16];
    SVECTOR puff_velocity[2][16];
    SVECTOR rays[2][16];
    SVECTOR flecks[2][64];
    SVECTOR fleck_velocity[2][64];
    SVECTOR sparks[2][16];
    u32 elapsed;
    u32 flight[2];
    u16 puff_frame[2][16];
    u16 spark_frame[2][16];
    u16 tail[2];
    u16 unknown_10E4;
    u16 spark_count[2];
    u16 trail_head[2];
    u16 head;
    u16 arrived[2];
    u16 phase;
    s32 ambient;
    s32 texture[6];
    u8 bolt_r[2];
    u8 bolt_g[2];
    u8 bolt_b[2];
    u8 glow_r[2];
    u8 glow_g[2];
    u8 glow_b[2];
    u8 ray_r[2];
    u8 ray_g[2];
    u8 ray_b[2];
} Model846State;

extern GsIMAGE D_8013EAD8[];
extern Model846Config D_8013EB80[];

s32 func_8013B004(u8 *context, s32 command);

#endif

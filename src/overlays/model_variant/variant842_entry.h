#ifndef MEMORIES_DECOMP_MODEL_VARIANT842_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT842_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x44-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of quartered sprites and
 * spinning strips around each bolt, the smoke puffs, the impact glow, the
 * target jitter, the sparks, the two fade steps and the three bolt
 * start times. */
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
    u16 jitter;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_2F;
    u8 ray_fade, unknown_31;
    u16 unknown_32[5];
    u16 start[3];
    u16 unknown_42;
} Model842Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, smoke puffs and
 * their velocities, sparks, the sixteen ray angles and the ray length;
 * the timer, the per-bolt flight counters, puff and spark frames, trail
 * lengths, spark counts and trail heads; the bolt count, the arrival
 * flags, the phase, the ambient level, the five textures, the fixed ring
 * texture and the per-bolt bolt, glow and ray colours. */
typedef struct {
    Model842Config *G32 config;
    SVECTOR pos[3];
    SVECTOR step[3];
    SVECTOR angle[3];
    SVECTOR ring[3][32];
    SVECTOR target[3];
    SVECTOR trail[3][32];
    SVECTOR origin[3];
    SVECTOR puffs[3][8];
    SVECTOR puff_velocity[3][8];
    SVECTOR sparks[3][8];
    SVECTOR rays[3][16];
    u32 ray_length[3];
    u32 elapsed;
    u32 flight[3];
    u16 puff_frame[3][8];
    u16 spark_frame[3][8];
    u16 tail[3];
    u16 unknown_ABE;
    u16 spark_count[3];
    u16 trail_head[3];
    u16 head;
    u16 arrived[3];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u16 ring_tpage;
    u16 ring_clut;
    u8 bolt_r[3];
    u8 bolt_g[3];
    u8 bolt_b[3];
    u8 glow_r[3];
    u8 glow_g[3];
    u8 glow_b[3];
    u8 ray_r[3];
    u8 ray_g[3];
    u8 ray_b[3];
} Model842State;

extern GsIMAGE D_8013E674[];
extern Model842Config D_8013E700[];

s32 func_8013B004(u8 *context, s32 command);

#endif

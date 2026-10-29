#ifndef MEMORIES_DECOMP_MODEL_VARIANT824_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT824_ENTRY_H

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
#include "../../psyq/abs.h"

/* One 0x3C-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, its rays, the sparks,
 * the two fade steps, the spawn period and the start and end times. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part;
    u8 unknown_0D;
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
    u16 period;
    u16 start;
    u16 end;
} Model824Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, sixteen-point trail, origin, smoke puffs and
 * their velocities, impact rays and sparks; the inner and outer shock-ring
 * points; the timer, flight counters, frame count, puff and spark frames,
 * trail lengths, spark counts and trail heads; the bolt count, arrival
 * flags, phase, per-bolt shock-ring growth, ambient level, the shock-ring
 * texture page and CLUT, the textures and the per-bolt colours. */
typedef struct {
    Model824Config *G32 config;
    SVECTOR pos[28];
    SVECTOR step[28];
    SVECTOR angle[28];
    SVECTOR ring[28][8];
    SVECTOR target[28];
    SVECTOR trail[28][16];
    SVECTOR origin[28];
    SVECTOR puffs[28][8];
    SVECTOR puff_velocity[28][8];
    SVECTOR rays[28][8];
    SVECTOR fan_inner[16];
    SVECTOR fan_outer[16];
    SVECTOR sparks[28][8];
    u32 elapsed;
    u32 flight[28];
    u32 frames;
    u16 puff_frame[28][8];
    u16 spark_frame[28][8];
    u16 tail[28];
    u16 unknown_3A94;
    u16 spark_count[28];
    u16 trail_head[28];
    u16 head;
    u16 arrived[28];
    u16 phase;
    u16 growth[28];
    s32 ambient;
    u16 fan_tpage;
    u16 fan_clut;
    s32 texture[5];
    u8 bolt_r[28];
    u8 bolt_g[28];
    u8 bolt_b[28];
    u8 glow_r[28];
    u8 glow_g[28];
    u8 glow_b[28];
    u8 ray_r[28];
    u8 ray_g[28];
    u8 ray_b[28];
} Model824State;

extern GsIMAGE D_8013EAE4[];
extern Model824Config D_8013EB70[];

s32 func_8013B004(u8 *context, s32 command);

#endif

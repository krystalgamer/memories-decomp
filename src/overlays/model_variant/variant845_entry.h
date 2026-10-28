#ifndef MEMORIES_DECOMP_MODEL_VARIANT845_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT845_ENTRY_H

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

/* One 0x48-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolt leaves from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around the bolt, the smoke puffs, the impact glow, its rays, the shock
 * fan, the sparks, the two fade steps, the two fan growth rates and the
 * start time. */
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
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_39;
    u8 ray_fade, unknown_3B;
    u32 grow;
    u32 grow_late;
    u16 start;
    u16 unknown_46;
} Model845Config;

/* The work area: the command row; for each of the two bolts its position, step, strip
 * angle, ring offsets, target, 24-point trail, origin, smoke puffs and
 * their velocities and impact rays; the sixteen inner and outer fan
 * points; the sparks; the fan growth, the timer, the flight counters, the
 * frame count, puff and spark frames, trail lengths, spark counts and
 * trail heads; the launch flag, the arrival flags, the phase, the ambient
 * level, the textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    Model845Config *G32 config;
    SVECTOR pos[2];
    SVECTOR step[2];
    SVECTOR angle[2];
    SVECTOR ring[2][18];
    SVECTOR target[2];
    SVECTOR trail[2][24];
    SVECTOR origin[2];
    SVECTOR puffs[2][8];
    SVECTOR puff_velocity[2][8];
    SVECTOR rays[2][32];
    SVECTOR fan[16][2];
    SVECTOR sparks[2][16];
    u32 growth;
    u32 elapsed;
    u32 flight[2];
    u32 frames;
    u16 puff_frame[2][8];
    u16 spark_frame[2][8];
    u16 tail[2];
    u16 unknown_84C;
    u16 spark_count[2];
    u16 trail_head[2];
    u16 head;
    u16 arrived[2];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[2];
    u8 bolt_g[2];
    u8 bolt_b[2];
    u8 glow_r[2];
    u8 glow_g[2];
    u8 glow_b[2];
    u8 ray_r[2];
    u8 ray_g[2];
    u8 ray_b[2];
} Model845State;

extern GsIMAGE D_8013EACC[];
extern Model845Config D_8013EB58[];

s32 func_8013B004(u8 *context, s32 command);

#endif

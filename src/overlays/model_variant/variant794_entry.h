#ifndef MEMORIES_DECOMP_MODEL_VARIANT794_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT794_ENTRY_H

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
 * around each bolt, the smoke puffs, the impact glow, its rays and sparks,
 * the two fade steps and the spawn period, start and end. */
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
    u16 period;
    u16 start;
    u16 end;
} Model794Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, smoke puffs
 * and their velocities, impact rays and sparks; the timers, the per-bolt
 * flight counters, puff and spark frames, trail lengths, spark counts and
 * trail heads; the bolt count, the arrival flags, the phase, the ambient
 * level, the five textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    Model794Config *G32 config;
    SVECTOR pos[16];
    SVECTOR step[16];
    SVECTOR angle[16];
    SVECTOR ring[16][16];
    SVECTOR target[16];
    SVECTOR trail[16][32];
    SVECTOR origin[16];
    SVECTOR puffs[16][8];
    SVECTOR puff_velocity[16][8];
    SVECTOR rays[16][16];
    SVECTOR sparks[16][8];
    u32 elapsed;
    u32 flight[16];
    u32 frames;
    u16 puff_frame[16][8];
    u16 spark_frame[16][8];
    u16 tail[16];
    u16 unknown_30EC;
    u16 spark_count[16];
    u16 trail_head[16];
    u16 head;
    u16 arrived[16];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[16];
    u8 bolt_g[16];
    u8 bolt_b[16];
    u8 glow_r[16];
    u8 glow_g[16];
    u8 glow_b[16];
    u8 ray_r[16];
    u8 ray_g[16];
    u8 ray_b[16];
} Model794State;

extern GsIMAGE gModelVariant794_D3864[];
extern Model794Config gModelVariant794_D38F0[];

s32 ModelVariant794_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT799_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT799_ENTRY_H

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

/* One 0x4C-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring and strip shapes, the smoke
 * puffs, the impact glow, the target jitter, the shock-fan shape, the
 * sparks, the fade steps, the shock-fan growth rates and the six bolt
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
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_35;
    u8 ray_fade, unknown_37;
    u32 grow;
    u32 grow_late;
    u16 start[6];
} Model799Config;

/* One bolt's colour bytes, padded to a word. */
typedef struct {
    u8 r, g, b, pad;
} Model799Colour;

/* The work area for up to six bolts: positions, steps, strip angles, ring
 * offsets, targets, 32-point trails and origins; two sets of 64 falling
 * debris pieces with their velocities; smoke puffs; the sixteen-segment
 * shock fan; the timer, flight counters, fan growth, flash heights and
 * frames; per-bolt frame and trail counters; the textures and colours.
 * The init writes its spark table over the timers (see the source), and
 * the sparks are never drawn. */
typedef struct {
    Model799Config *G32 config;
    SVECTOR pos[6];
    SVECTOR step[6];
    SVECTOR angle[6];
    SVECTOR ring[6][32];
    SVECTOR target[6];
    SVECTOR trail[6][32];
    SVECTOR origin[6];
    SVECTOR shards[6][64];
    SVECTOR shard_velocity[6][64];
    SVECTOR puffs[6][8];
    SVECTOR puff_velocity[6][8];
    SVECTOR chips[6][64];
    SVECTOR chip_velocity[6][64];
    SVECTOR fan[16][2];
    union {
        SVECTOR sparks[8];
        struct {
            u32 elapsed;
            u32 flight[6];
            u32 growth[6];
            u16 ray_length[6];
            u16 flash_frame[6];
        } timers;
    } u;
    u16 puff_frame[6][8];
    u16 spark_frame[6][8];
    u16 tail[6];
    u16 unknown_420C;
    u16 spark_count[6];
    u16 trail_head[6];
    u16 head;
    u16 arrived[6];
    u16 phase;
    s32 ambient;
    u16 ring_tpage;
    u16 ring_clut;
    s32 texture[7];
    Model799Colour bolt[6];
    Model799Colour glow[6];
} Model799State;

extern GsIMAGE D_8013ED7C[];
extern Model799Config D_8013EE40[];

s32 func_8013B004(u8 *context, s32 command);

#endif

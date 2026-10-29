#ifndef MEMORIES_DECOMP_MODEL_VARIANT785_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT785_ENTRY_H

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

/* One 0x50-byte command row: the shared sprite colour, the model point the
 * projectiles leave from, the smoke spread, the per-group projectile size,
 * count, spread and lift, the flight steps, the gravity, the ring of sprites
 * around each projectile, the trail strip width and swirl radius, the fade
 * steps, the two impact fans and their spark bursts, the fan growth rates,
 * the smoke puffs, the flash size, the start time and the end time. */
typedef struct {
    u8 r, g, b;
    u8 part;
    s16 puff_spread;
    s16 size[2];
    s16 count[2];
    s16 steps;
    s16 gravity;
    s16 spread[2];
    s16 lift[2];
    s16 ring_count;
    s16 strip_width;
    s16 strip_radius;
    s16 ring_size;
    u8 fade, unknown_23;
    u8 glow_fade, unknown_25;
    s16 fan_inner;
    s16 fan_outer;
    s16 fan_depth;
    s16 grow;
    u16 unknown_2E;
    u32 grow_late;
    s16 spark_size;
    s16 spark_count;
    s16 fan2_inner;
    s16 fan2_outer;
    s16 fan2_depth;
    s16 spark2_size;
    s16 spark2_count;
    s16 puff_count;
    s16 puff_size;
    s16 puff_speed;
    s16 flash_size;
    s16 start;
    s16 end;
    u16 unknown_4E;
} Model785Config;

/* The work area: the command row; per projectile and group its position,
 * step, sprite ring and 64-point trail; the inner and outer points of both
 * impact fans and their sparks; the smoke origin, puffs and velocities; the
 * timer, the fan growths, trail heads, spark frames, puff frames, flash
 * frame, phase, arrival flags, trail lengths, spark counts, ambient level,
 * textures and the projectile and fan colours. */
typedef struct {
    Model785Config *G32 config;
    SVECTOR pos[8][2];
    SVECTOR step[8][2];
    SVECTOR ring[8][8][2];
    SVECTOR trail[8][64][2];
    SVECTOR fan_inner[16];
    SVECTOR fan_outer[16];
    SVECTOR sparks[32];
    SVECTOR fan2_inner[16];
    SVECTOR fan2_outer[16];
    SVECTOR sparks2[32];
    SVECTOR origin;
    SVECTOR puffs[8];
    SVECTOR puff_velocity[8];
    u32 elapsed;
    /* The fan growth of the first and second groups; the second group's
     * second layer reads the first group's words by its own index, running
     * on into the second group's (see the source), through the flat view. */
    union {
        struct {
            u32 growth[3];
            u32 growth2[8];
        } s;
        u32 all[11];
    } fans;
    u16 trail_head[8][2];
    u16 spark_frame[3][32];
    u16 spark2_frame[8][32];
    u16 puff_frame[8];
    u16 flash_frame;
    u16 phase;
    u16 arrived[8][2];
    u16 tail[8][2];
    u16 strip[8][2];
    u16 spark_count[3];
    u16 spark2_count[9];
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[8][2];
    u8 bolt_g[8][2];
    u8 bolt_b[8][2];
    u8 glow_r[3];
    u8 glow_g[3];
    u8 glow_b[3];
    u8 glow2_r[8];
    u8 glow2_g[8];
    u8 glow2_b[8];
} Model785State;

extern GsIMAGE D_8013ECBC[];
extern Model785Config D_8013ED48[];

s32 func_8013B004(u8 *context, s32 command);

#endif

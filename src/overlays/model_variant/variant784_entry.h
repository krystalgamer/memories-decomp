#ifndef MEMORIES_DECOMP_MODEL_VARIANT784_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT784_ENTRY_H

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

/* One 0x44-byte command row: the projectile, glow and second glow colours,
 * the model point the shots leave from, the projectile size, the flight
 * steps, the ring count, the swirl strip width and radius, the ring sprite
 * size, the target spread, the smoke spread, puff count, puff size and puff
 * speed, the flash size, the fade, the spark size, the fan radii and depth,
 * the fan growth rates, the spark count, the second fan radii and the five
 * shot times. */
typedef struct {
    u8 r, g, b;
    u8 glow_r, glow_g, glow_b;
    u8 glow2_r, glow2_g, glow2_b;
    u8 part;
    s16 size;
    s16 unknown_0C;
    s16 unknown_0E;
    s16 steps;
    s16 ring_count;
    s16 strip_width;
    s16 strip_radius;
    s16 ring_size;
    s16 target_spread;
    s16 puff_spread;
    s16 puff_count;
    s16 puff_size;
    s16 puff_speed;
    s16 flash_size;
    s16 fade;
    s16 spark_size;
    s16 fan_inner;
    s16 fan_outer;
    s16 fan_depth;
    s16 grow;
    s16 grow_late;
    s16 spark_count;
    s16 fan2_inner;
    s16 fan2_outer;
    s16 start[5];
} Model784Config;

/* The work area: the command row; per shot its position, 16-point trail,
 * step, heading, ring of sprites, smoke origin, puffs and velocities and
 * sparks; the shared fan rings; the puff, flash and spark frames; the
 * textures, ambient level, fan growths, timer, shots fired, shot states,
 * phase, trail heads and lengths, spark counts and the projectile and fan
 * colours. */
typedef struct {
    Model784Config *G32 config;
    SVECTOR pos[5];
    SVECTOR trail[5][16];
    SVECTOR step[5];
    SVECTOR heading[5];
    SVECTOR ring[5][64];
    SVECTOR origin[5];
    SVECTOR puffs[5][8];
    SVECTOR puff_velocity[5][8];
    SVECTOR fan_inner[16];
    SVECTOR fan_outer[16];
    SVECTOR sparks[5][8];
    SVECTOR fan2_inner[16];
    SVECTOR fan2_outer[16];
    u16 puff_frame[5][8];
    u16 flash_frame[5];
    u16 spark_frame[5][8];
    s32 texture[5];
    s32 ambient;
    s32 growth[5];
    s32 elapsed;
    s16 fired;
    s16 state[5];
    s16 phase;
    s16 trail_head[5];
    u16 tail[5];
    u16 spark_count[5];
    u8 bolt_r[5];
    u8 bolt_g[5];
    u8 bolt_b[5];
    u8 glow_r[5];
    u8 glow_g[5];
    u8 glow_b[5];
    u8 glow2_r[5];
    u8 glow2_g[5];
    u8 glow2_b[5];
} Model784State;

extern GsIMAGE gModelVariant784_D4090[];
extern Model784Config gModelVariant784_D411C[];

s32 ModelVariant784_HandleRequest(u8 *context, s32 command);

#endif

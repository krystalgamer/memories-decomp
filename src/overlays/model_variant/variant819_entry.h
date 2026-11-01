#ifndef MEMORIES_DECOMP_MODEL_VARIANT819_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT819_ENTRY_H

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

/* One 0x48-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * fall length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, the landing jitter,
 * the shock fan, the sparks, the fade step, the two fan growth rates and
 * the four bolt start times. */
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
    u16 jitter;
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_35;
    u8 unknown_36, unknown_37;
    u32 grow;
    u32 grow_late;
    u16 start[4];
} Model819Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, 64 shards and their
 * velocities, smoke puffs and their velocities, 64 debris sprites and
 * their velocities; the sixteen inner and outer fan points; the timer, the
 * per-bolt flight counters and fan growth, flash swell and frame, puff and
 * spark frames, trail lengths, spark counts and trail heads; the bolt
 * count, the arrival flags, the phase, the ambient level, the fixed ring
 * texture, the seven textures and the per-bolt bolt and glow colours. */
typedef struct {
    Model819Config *G32 config;
    SVECTOR pos[4];
    SVECTOR step[4];
    SVECTOR angle[4];
    SVECTOR ring[4][32];
    SVECTOR target[4];
    SVECTOR trail[4][32];
    SVECTOR origin[4];
    SVECTOR shards[4][64];
    SVECTOR shard_velocity[4][64];
    SVECTOR puffs[4][8];
    SVECTOR puff_velocity[4][8];
    SVECTOR debris[4][64];
    SVECTOR debris_velocity[4][64];
    SVECTOR fan[16][2];
    /* The entry seeds the sparks without a bolt index, so they overlay the
     * timer and the counters that follow it. */
    union {
        struct {
            u32 elapsed;
            union {
                struct {
                    u32 flight[4];
                    u32 growth[4];
                } split;
                u32 rows[2][4];
            } timers;
            u16 swell[4];
            u16 flash_frame[4];
            u16 puff_frame[4][8];
        } s;
        SVECTOR sparks[8];
    } state;
    u16 spark_frame[4][8];
    u16 tail[4];
    u16 unknown_2C60;
    u16 spark_count[4];
    u16 trail_head[4];
    u16 head;
    u16 arrived[4];
    u16 phase;
    s32 ambient;
    u16 ring_tpage;
    u16 ring_clut;
    s32 texture[7];
    CVECTOR bolt[4];
    CVECTOR glow[4];
} Model819State;

extern GsIMAGE gModelVariant819_D3CC8[];
extern Model819Config gModelVariant819_D3D8C[];

s32 ModelVariant819_HandleRequest(u8 *context, s32 command);

#endif

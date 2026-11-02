#ifndef MEMORIES_DECOMP_MODEL_VARIANT825_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT825_ENTRY_H

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

/* One 0x42-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, its rays, the sparks,
 * the streaks, the two fade steps, the spawn period and the start and end
 * times. */
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
    u16 streak_spread;
    u16 streak_width;
    u16 streak_height;
    u8 fade, unknown_39;
    u8 ray_fade, unknown_3B;
    u16 period;
    u16 start;
    u16 end;
} Model825Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, sixteen-point trail, origin, smoke puffs and
 * their velocities, impact rays, sparks and streaks; the timer, flight
 * counters, frame count, puff, spark and streak frames, trail lengths,
 * spark and streak counts, the trail heads, bolt count and arrival flags,
 * phase, ambient level, the textures and the per-bolt colours. */
typedef struct {
    Model825Config *G32 config;
    SVECTOR pos[10];
    SVECTOR step[10];
    SVECTOR angle[10];
    SVECTOR ring[10][8];
    SVECTOR target[10];
    SVECTOR trail[10][16];
    SVECTOR origin[10];
    SVECTOR puffs[10][8];
    SVECTOR puff_velocity[10][8];
    SVECTOR rays[10][8];
    SVECTOR sparks[10][8];
    SVECTOR streaks[10][16];
    u32 elapsed;
    u32 flight[10];
    u32 frames;
    u16 puff_frame[10][8];
    u16 spark_frame[10][8];
    u16 streak_frame[10][16];
    u16 tail[10];
    u16 unknown_1AD8;
    u16 spark_count[10];
    u16 streak_count[10];
    /* The entry reads the trail heads through the flat view with the
     * finished sixteen-step loop counter, landing in the arrival flags. */
    union {
        struct {
            u16 trail_head[10];
            u16 head;
            u16 arrived[10];
        } s;
        u16 flat[21];
    } bolts;
    u16 phase;
    s32 ambient;
    s32 texture[6];
    u8 bolt_r[10];
    u8 bolt_g[10];
    u8 bolt_b[10];
    u8 glow_r[10];
    u8 glow_g[10];
    u8 glow_b[10];
    u8 ray_r[10];
    u8 ray_g[10];
    u8 ray_b[10];
} Model825State;

extern GsIMAGE gModelVariant825_D3D0C[];
extern Model825Config gModelVariant825_D3D98[];

s32 ModelVariant825_HandleRequest(u8 *context, s32 command);

#endif

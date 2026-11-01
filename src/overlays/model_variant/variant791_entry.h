#ifndef MEMORIES_DECOMP_MODEL_VARIANT791_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT791_ENTRY_H

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

/* One 0x58-byte command row: the muzzle sprite colour, the bolt, glow and
 * ray colours, the model point the bolts leave from, the muzzle size, the
 * flight lengths of the two volleys, the bolt counts of the two volleys,
 * the ring of sprites and spinning strips around each bolt, the second
 * volley's ground spread, the smoke puffs, the impact glow, its rays, the
 * shock fan, the sparks, the two fade steps, the two fan growth rates and
 * the start times of the two volleys. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 ray_r, ray_g, ray_b;
    u8 part, unknown_0D;
    u16 size;
    u16 unknown_10;
    u16 steps;
    u16 second_steps;
    u16 count;
    u16 second_count;
    u16 ring_count;
    u16 unknown_1C;
    u16 strip_width;
    u16 unknown_20;
    u16 strip_radius;
    u16 unknown_24;
    u16 ring_size;
    u16 unknown_28;
    u16 second_spread;
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
    u8 fade, unknown_49;
    u8 ray_fade, unknown_4B;
    u32 grow;
    u32 grow_late;
    u16 start;
    u16 second_start;
} Model791Config;

/* The work area: the command row; for each of up to fifteen bolts its
 * position, step, strip angle, ring offsets, target and 32-point trail; the
 * two volleys' origins; per bolt the smoke puffs, their velocities and the
 * impact rays; the sixteen inner and outer fan points; the first volley's
 * sparks; the timer, the flight counters, the second volley's fan growth,
 * the puff and spark frames, the trail lengths, the launched bolt count,
 * the trail heads, the phase, the arrival flags, the ambient level, the
 * textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    Model791Config *G32 config;
    SVECTOR pos[15];
    SVECTOR step[15];
    SVECTOR angle[15];
    SVECTOR ring[15][32];
    SVECTOR target[15];
    SVECTOR trail[15][32];
    SVECTOR origin;
    SVECTOR second_origin;
    SVECTOR puffs[15][8];
    SVECTOR puff_velocity[15][8];
    SVECTOR rays[15][16];
    SVECTOR fan[16][2];
    SVECTOR sparks[3][8];
    u32 elapsed;
    u32 flight[15];
    u32 growth[12];
    u16 puff_frame[15][8];
    u16 spark_frame[3][8];
    u16 tail[15];
    u16 launched;
    u16 trail_head[15];
    u16 phase;
    u16 arrived[15];
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[15];
    u8 bolt_g[15];
    u8 bolt_b[15];
    u8 glow_r[15];
    u8 glow_g[15];
    u8 glow_b[15];
    u8 ray_r[15];
    u8 ray_g[15];
    u8 ray_b[15];
} Model791State;

extern GsIMAGE gModelVariant791_D3EB0[];
extern Model791Config gModelVariant791_D3F3C[];

s32 ModelVariant791_HandleRequest(u8 *context, s32 command);

#endif

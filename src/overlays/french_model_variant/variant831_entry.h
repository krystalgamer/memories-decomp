#ifndef MEMORIES_FRENCH_MODEL_VARIANT831_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT831_ENTRY_H

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

/* One 0x48-byte command row: the header-792 row with the ray fan replaced by
 * a two-ring flare (inner and outer radii and depth), byte fade steps, the
 * near and far flare growth rates and a third launch time. */
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
    u16 field_26;
    u16 ray_inner;
    u16 ray_outer;
    u16 ray_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_35;
    u8 ray_fade, unknown_37;
    u32 grow_near;
    u32 grow_far;
    u16 start;
    u16 second_start;
    u16 third_start;
    u16 mode;
} Model831Config;

/* The work area: the header-792 layout for three bolts, with a per-bolt
 * flare timer after the flight counters, a shared 16-segment ray ring and
 * the flare texture page and CLUT after the five textures. */
typedef struct {
    Model831Config *G32 config;
    SVECTOR pos[3];
    SVECTOR step[3];
    SVECTOR angle[3];
    SVECTOR ring[3][32];
    SVECTOR target[3];
    SVECTOR trail[3][32];
    SVECTOR origin[3];
    SVECTOR puffs[3][8];
    SVECTOR puff_velocity[3][8];
    SVECTOR rays[16][2];
    SVECTOR sparks[3][8];
    u32 elapsed;
    u32 flight[3];
    u32 flare_scale[3];
    u16 puff_frame[3][8];
    u16 spark_frame[3][8];
    u16 tail[3];
    u16 unknown_3994;
    u16 spark_count[3];
    u16 trail_head[3];
    u16 head;
    u16 arrived[3];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u16 flare_tpage;
    u16 flare_clut;
    u8 bolt_r[3];
    u8 bolt_g[3];
    u8 bolt_b[3];
    u8 glow_r[3];
    u8 glow_g[3];
    u8 glow_b[3];
    u8 ray_r[3];
    u8 ray_g[3];
    u8 ray_b[3];
} Model831State;

extern GsIMAGE D_8013E818[];
extern Model831Config D_8013E8A4[];

#endif

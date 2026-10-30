#ifndef MEMORIES_DECOMP_MODEL_VARIANT790_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT790_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x24-byte command row: the star and burst colours, the model part
 * the stars leave from, the star scale, the flight time, the star count,
 * the burst glow size, the spark spread, count and size, the trail width,
 * the spawn period, the fade step and the start and end of the spawning. */
typedef struct {
    u8 r, g, b;
    u8 glow_r, glow_g, glow_b;
    u8 part, unknown_7;
    u16 scale;
    u16 travel_time;
    u16 count;
    u16 unknown_0E;
    u16 glow_size;
    u16 spark_spread;
    u16 spark_count;
    u16 spark_size;
    u16 trail_width;
    u16 period;
    u8 fade, unknown_1D;
    u16 unknown_1E;
    s16 start;
    s16 end;
} Model790Config;

/* The eleven corners of the star and the five quads built from them. */
typedef struct {
    SVECTOR v[11];
} Model790Shape;

/* The five quads, also walked as one flat run of 20 corner indices. */
typedef union {
    u8 q[5][4];
    u8 flat[20];
} Model790Faces;

/* The work area: per star its position, velocity and landing point, its
 * burst sparks with their velocities and its 64-point trail; the timer and
 * frame counter, the star count, the bounce flags, the phase, the trail
 * heads and drawn lengths, the ambient level, the two textures and the
 * per-star star, burst and trail intensities. */
typedef struct {
    Model790Config *G32 config;
    SVECTOR pos[14];
    SVECTOR velocity[14];
    SVECTOR target[14];
    SVECTOR sparks[14][16];
    SVECTOR spark_velocities[14][16];
    SVECTOR trail[14][64];
    u32 elapsed;
    u32 frames;
    u16 count;
    u16 bounced[14];
    u16 phase;
    u16 head[14];
    u16 drawn[14];
    s32 level;
    s32 texture[2];
    u8 r[14];
    u8 g[14];
    u8 b[14];
    u8 glow_r[14];
    u8 glow_g[14];
    u8 glow_b[14];
    u8 trail_level[14];
} Model790State;

extern GsIMAGE D_8013CF2C[2];
extern Model790Config D_8013CF64[];

s32 func_8013B004(u8 *context, s32 command);

#endif

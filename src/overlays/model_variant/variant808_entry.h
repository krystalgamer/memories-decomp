#ifndef MEMORIES_DECOMP_MODEL_VARIANT808_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT808_ENTRY_H

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

/* One 0x2E-byte command row: the comet colour, the model part it starts
 * from, the comet scale, its travel time, the sparkle size, spread and
 * count, the trail width, the burst glow size, the ember count, spread and
 * gravity, the inner and outer ring radii, the ember size, the fade-in and
 * fade-out steps, and the timing of the start, the launch and the sparkle
 * stop. */
typedef struct {
    u8 r, g, b;
    u8 unknown_3, unknown_4, unknown_5;
    u8 part, unknown_7;
    u16 comet_scale;
    u16 travel_time;
    u16 sparkle_size;
    u16 sparkle_spread;
    u16 sparkle_count;
    u16 trail_width;
    u16 glow_size;
    u16 ember_count;
    u16 ember_spread;
    u16 gravity;
    u16 inner_radius;
    u16 outer_radius;
    u16 ember_size;
    u8 fade_in, unknown_23;
    u8 fade_out, unknown_25;
    u16 unknown_26;
    s16 start;
    s16 launch;
    s16 sparkle_end;
} Model808Config;

/* The eleven corners of the comet star and the five quads built from them. */
typedef struct {
    SVECTOR v[11];
} Model808Shape;

/* The five quads, also walked as one flat run of 20 corner indices. */
typedef union {
    u8 q[5][4];
    u8 flat[20];
} Model808Faces;

/* The work area: the comet position and velocity, the sparkles with their
 * velocities, the falling embers with their velocities, the comet trail,
 * the ring of paired points, the drifting embers with their velocities, the
 * timer, the ring scale, the sparkle frames, the phase, the sparkle count,
 * the drawn trail length, the trail head, the ambient level, the textures,
 * the comet colour and the trail enable. */
typedef struct {
    Model808Config *G32 config;
    SVECTOR center;
    SVECTOR velocity;
    SVECTOR sparkles[32];
    SVECTOR sparkle_velocities[32];
    SVECTOR embers[32];
    SVECTOR ember_velocities[32];
    SVECTOR trail[64];
    SVECTOR ring[16][2];
    SVECTOR drifts[32];
    SVECTOR drift_velocities[32];
    u32 elapsed;
    u32 ring_scale;
    u16 frames[32];
    u16 phase;
    u16 count;
    u16 drawn;
    u16 head;
    s32 level;
    s32 texture[5];
    union {
        struct {
            u8 r, g, b;
            u8 trail_on;
        } c;
        u32 word;
    } color;
} Model808State;

extern GsIMAGE gModelVariant808_D2228[5];
extern Model808Config gModelVariant808_D22B4[];

s32 ModelVariant808_HandleRequest(u8 *context, s32 command);

#endif

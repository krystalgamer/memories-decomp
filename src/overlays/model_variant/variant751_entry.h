#ifndef MEMORIES_DECOMP_MODEL_VARIANT751_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT751_ENTRY_H

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

/* One 0x2E-byte command row: the orb colour and the model part it starts
 * from, the flight steps, the orb, cloud, burst and spark sizes, the rising
 * lift, the swing radius of the spinning rays, the ring radii and depth, the
 * spark spread, the fade-out and fade-in steps, the two ring growth rates and
 * the launch, bob, start and two camera times. */
typedef struct {
    u8 r, g, b, part;
    u16 steps;
    u16 size;
    u16 cloud_size;
    u16 lift;
    u16 swing;
    u16 ring_outer;
    u16 ring_inner;
    u16 ring_depth;
    u16 burst_size;
    u16 spark_size;
    u16 spread;
    u8 fade, pad_1B;
    u16 unknown_1C;
    u8 fade_in, pad_1F;
    u16 grow;
    u16 slow_grow;
    s16 launch_time;
    s16 bob_time;
    s16 start_time;
    s16 camera_time[2];
} Model751Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model751Color;

/* The work area: the command row, the orb position and velocity, the cloud
 * offsets, the two ring edges, the sparks and their velocities, the bob, the
 * timer, the ray spin and lift, the phase and state, the ring scale, the
 * ambient level, the textures and the shared colour. */
typedef struct {
    Model751Config *G32 config;
    SVECTOR pos;
    SVECTOR velocity;
    SVECTOR offsets[16];
    SVECTOR ring[16][2];
    SVECTOR sparks[32];
    SVECTOR spark_velocity[32];
    s16 bob, pad_396;
    u32 elapsed;
    s16 spin;
    u16 lift;
    u16 phase;
    u16 state;
    u32 scale;
    s32 ambient;
    s32 texture[3];
    Model751Color color;
} Model751State;

extern GsIMAGE D_8013CA50[];
extern Model751Config D_8013CAA4[];

s32 func_8013B004(u8 *context, s32 command);

#endif

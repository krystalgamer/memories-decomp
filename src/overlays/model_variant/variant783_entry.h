#ifndef MEMORIES_DECOMP_MODEL_VARIANT783_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT783_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x40-byte command row: the orb's final colour, the ray and ring
 * colours, the slot part it starts from, its size and first lift, the
 * particle counts, speeds and divisors, the ray and ring shapes, the fade
 * rates and the timeline. */
typedef struct {
    u8 orb_r, orb_g, orb_b;
    u8 ray_r, ray_g, ray_b;
    u8 ring_r, ring_g, ring_b;
    u8 part;
    s16 orb_size;
    s16 lift;
    s16 gather_count;
    s16 gather_speed;
    s16 gather_divisor;
    s16 travel_divisor;
    s16 ray_count;
    s32 ray_length;
    s16 ray_width;
    s16 burst_count;
    s16 burst_divisor;
    s16 burst_speed;
    s16 ring_outer;
    s16 ring_inner;
    s16 spark_speed;
    s16 spark_count;
    s16 fade_rate;
    s16 flash_rate;
    s16 ring_fade_rate;
    s16 start;
    s16 ring_start;
    s16 unknown_36;
    s32 burst_start;
    s32 unknown_3C;
} Model783Config;

/* The work area: the command row, the orb's position and step, the gathering
 * (and later bursting) particles with their steps, the ray spins, the two ring
 * edges, the sparks with their steps, the ring centre, the sparks' screen
 * points and depths, the textures, the ambient level, the timers and the
 * colours of the orb, the flash, the particles and the ring. */
typedef struct {
    Model783Config *G32 config;
    SVECTOR position;
    SVECTOR step;
    SVECTOR particles[128];
    SVECTOR particle_steps[128];
    SVECTOR spins[128];
    SVECTOR ring_outer[16];
    SVECTOR ring_inner[16];
    SVECTOR sparks[32];
    SVECTOR spark_steps[32];
    SVECTOR center;
    DVECTOR spark_screen[32];
    s32 texture[2];
    s32 level;
    s32 elapsed;
    s32 growth;
    s16 phase;
    s16 gathered;
    u16 center_set;
    u16 spark_depth[32];
    u8 orb_r, orb_g, orb_b;
    u8 flash;
    u8 dots;
    u8 ring_r, ring_g, ring_b;
} Model783State;

extern GsIMAGE D_8013D500[];
extern Model783Config D_8013D538[];

s32 func_8013B004(u8 *context, s32 command);

#endif

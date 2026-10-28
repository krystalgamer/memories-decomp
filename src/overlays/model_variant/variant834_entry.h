#ifndef MEMORIES_DECOMP_MODEL_VARIANT834_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT834_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x26-byte command row: the orb colour, the burst colour, the beam width
 * and height, the orb and spark sizes, the four ring radii, the ring drop and
 * depth, the source part, and the phase timings. */
typedef struct {
    u8 r, g, b;
    u8 burst_r, burst_g, burst_b;
    u16 beam_width, beam_height;
    u16 orb_size, spark_size;
    u16 top_radius, base_radius, base_drop;
    u16 burst_radius, flare_radius, flare_depth;
    u8 part, pad_1b;
    u16 delay, orb_time, launch_time, fly_time, spawn_time;
} Model834Config;

/* A colour that the entry tests as a 24-bit word. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model834Color;

/* The work area: the command row, the beam position, its rise and flight
 * velocities and rotation, the orbs and their velocities, the beam trail, the
 * four rings, the sparks and their velocities, the elapsed time, the frame,
 * the burst scale, the phase, the orb states, the orb and trail counts, the
 * ambient level, the ring texture, the textures, the beam colours, the orb
 * colours and the burst colour. */
typedef struct {
    Model834Config *G32 config;
    SVECTOR position;
    SVECTOR rise;
    SVECTOR flight;
    SVECTOR rotation;
    SVECTOR orbs[64];
    SVECTOR orb_velocities[64];
    SVECTOR trail[8];
    SVECTOR top[16];
    SVECTOR base[16];
    SVECTOR burst[16];
    SVECTOR flare[16];
    SVECTOR sparks[32];
    SVECTOR spark_velocities[32];
    u32 elapsed;
    s32 frame;
    u32 scale;
    u16 phase;
    u16 states[64];
    u16 unused;
    u16 orb_count;
    u16 trail_count;
    s32 level;
    u16 ring_tpage;
    u16 ring_clut;
    s32 texture[3];
    Model834Color beam_top;
    Model834Color beam_base;
    CVECTOR colors[64];
    Model834Color burst_color;
} Model834State;

extern GsIMAGE D_8013D620[];
extern Model834Config D_8013D674[];

s32 func_8013B004(u8 *context, s32 command);

#endif

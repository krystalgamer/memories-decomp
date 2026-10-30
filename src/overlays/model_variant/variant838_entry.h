#ifndef MEMORIES_DECOMP_MODEL_VARIANT838_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT838_ENTRY_H

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

/* One 0xE-byte command row: the effect colour, the source part, the core
 * fan radius, and the phase timings. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 radius;
    u16 delay, launch_time, arrive_time, end_time;
} Model838Config;

/* A colour that the entry tests as a 24-bit word. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model838Color;

/* The screen flash level packed with the shock-ring colour; the entry tests
 * the ring colour as the upper 24 bits of the word. */
typedef union {
    struct {
        u8 flash, r, g, b;
    } c;
    u32 word;
} Model838Flash;

/* The work area: the command row, the core fan rim, the comet trails, their
 * rotations, the comet position and velocity, the smoke puffs, sparks, impact
 * bursts, streaks and debris with their velocities, the shock-ring rims, the
 * launch point, the elapsed time, the frame, the phase, the scales, the trail
 * lengths, the particle counts, the ambient level, the ring texture, the
 * textures and the colours. */
typedef struct {
    Model838Config *G32 config;
    SVECTOR rim[32];
    SVECTOR trails[32][4];
    SVECTOR trail_rotations[32];
    SVECTOR position;
    SVECTOR velocity;
    SVECTOR puffs[32];
    SVECTOR puff_velocities[32];
    SVECTOR sparks[16];
    SVECTOR spark_velocities[16];
    SVECTOR bursts[32];
    SVECTOR streaks[32];
    SVECTOR debris[64];
    SVECTOR debris_velocities[64];
    SVECTOR ring_inner[16];
    SVECTOR ring_outer[16];
    SVECTOR origin;
    u32 elapsed;
    s32 frame;
    u16 phase;
    u16 scale;
    u16 ring_scale;
    u16 burst_scales[32];
    u16 trail_lengths[32];
    u16 trail_count;
    u16 puff_count;
    u16 spark_count;
    u16 burst_count;
    s32 level;
    u16 glow_tpage;
    u16 glow_clut;
    s32 texture[3];
    Model838Color core_color;
    Model838Color glow_color;
    Model838Color trail_colors[32];
    CVECTOR unused_color;
    CVECTOR puff_colors[32];
    CVECTOR spark_colors[16];
    Model838Color burst_color;
    CVECTOR burst_colors[32];
    Model838Flash flash;
} Model838State;

extern GsIMAGE D_8013E230[];
extern Model838Config D_8013E284[];

s32 func_8013B004(u8 *context, s32 command);

#endif

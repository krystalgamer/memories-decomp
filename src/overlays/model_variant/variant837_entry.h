#ifndef MEMORIES_DECOMP_MODEL_VARIANT837_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT837_ENTRY_H

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
#include "../../game/graphics_constants.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x0C-byte command row: the effect colour, the model part the glow
 * follows, the ring radius, the start time, the launch time and the time the
 * glow stops following the model. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 radius;
    u16 delay;
    u16 launch_time;
    u16 end_time;
} Model837Config;

/* A colour that the entry tests as a 24-bit word. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model837Color;

/* The work area: the command row, the 32-point glow ring, the 32 four-point
 * streaks and their rotations, the orb position and velocity, the rising
 * motes and their velocities, the orbiting motes and their velocities, the
 * burst offsets, the beam offsets, the scattered sparks and their
 * velocities, the elapsed time, the frame, the phase, the glow scale, the
 * burst scales, the streak lengths, the streak, mote, orbit and burst counts,
 * the ambient level, the ring texture, the textures, the ring glow, halo,
 * streak, core, mote, orbit, impact and burst colours and the flash level. */
typedef struct {
    Model837Config *G32 config;
    SVECTOR ring[32];
    SVECTOR streaks[32][4];
    SVECTOR rotations[32];
    SVECTOR position;
    SVECTOR velocity;
    SVECTOR motes[32];
    SVECTOR mote_velocities[32];
    SVECTOR orbits[16];
    SVECTOR orbit_velocities[16];
    SVECTOR bursts[32];
    SVECTOR beams[32];
    SVECTOR sparks[64];
    SVECTOR spark_velocities[64];
    u32 elapsed;
    s32 frame;
    u16 phase;
    u16 scale;
    u16 burst_scales[32];
    u16 lengths[32];
    u16 streak_count;
    u16 mote_count;
    u16 orbit_count;
    u16 burst_count;
    s32 level;
    u16 ring_tpage;
    u16 ring_clut;
    s32 texture[3];
    Model837Color glow;
    Model837Color halo;
    CVECTOR streak_colors[32];
    Model837Color core;
    CVECTOR mote_colors[32];
    CVECTOR orbit_colors[16];
    Model837Color impact;
    CVECTOR burst_colors[32];
    u8 flash;
} Model837State;

extern GsIMAGE gModelVariant837_D2F8C[];
extern Model837Config gModelVariant837_D2FE0[];

s32 ModelVariant837_HandleRequest(u8 *context, s32 command);

#endif

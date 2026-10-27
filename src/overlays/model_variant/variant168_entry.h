#ifndef MEMORIES_DECOMP_MODEL_VARIANT168_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT168_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x26-byte command row, picked by command % 100: the particle, burst and
 * cone colours, the slot part the particles start from and the cone sits on,
 * the particle half size, the endpoint spread, the burst half size, the rise
 * and spread of the four burst directions, the cone radius, the particle
 * count and spacing, the travel, fade, burst and cone durations, the particle
 * delay and the cone delay. */
typedef struct {
    u8 r, g, b, burst_r, burst_g, burst_b, cone_r, cone_g, cone_b, part;
    s16 half_size, spread, burst_half_size, rise, velocity_spread, cone_radius;
    s16 count, spacing, travel, fade, burst_duration, cone_duration, delay, cone_delay;
} Model168Config;

/* The 0x4F8-byte work area: the command row, 64 particle positions and 64
 * endpoints, four burst directions, the cone apex and its eight rim points
 * (four at the cone radius, four at a sixteenth of it), the eight cone
 * triangles as three point pointers each, nine texture words, the completion
 * flag and the elapsed time. */
typedef struct {
    Model168Config *G32 config;
    SVECTOR positions[64];
    SVECTOR endpoints[64];
    SVECTOR velocities[4];
    SVECTOR apex;
    SVECTOR rim[8];
    SVECTOR *G32 triangles[8][3];
    u32 texture[9];
    u8 completed;
    u8 unknown_4F1[3];
    s32 elapsed;
} Model168State;

extern GsIMAGE D_8013C1AC[];
extern Model168Config D_8013C2A8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

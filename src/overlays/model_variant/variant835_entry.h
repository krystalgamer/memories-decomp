#ifndef MEMORIES_DECOMP_MODEL_VARIANT835_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT835_ENTRY_H

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
#include "../../psyq/abs.h"

/* One 0x14-byte command row: the orb colour, the burst colour, the model
 * part the orbs leave from, the orb count, the target spread, the flight
 * length, the spark count and spread, and the launch time. */
typedef struct {
    u8 r, g, b;
    u8 burst_r, burst_g, burst_b;
    u8 part;
    u16 max_count;
    u16 spread;
    u16 steps;
    u16 spark_max;
    u16 spark_spread;
    u16 start;
} Model835Config;

/* The work area: the command row; per orb its position, step, sixteen-point
 * trail, eight spark offsets, burst scale, spark frames, state, trail start,
 * spark count, trail head, alpha and orb and burst colours; the timer,
 * phase, orb count, ambient level, the burst texture and the two loaded
 * textures. */
typedef struct {
    Model835Config *G32 config;
    SVECTOR pos[8];
    SVECTOR vel[8];
    SVECTOR trail[8][16];
    SVECTOR sparks[8][8];
    u32 elapsed;
    u32 scale[8];
    u16 spark_frame[8][8];
    u16 phase;
    u16 state[8];
    u16 count;
    u16 trail_start[8];
    u16 spark_count[8];
    u16 trail_head[8];
    s32 level;
    u16 burst_tpage;
    u16 burst_clut;
    s32 texture[2];
    u8 alpha[8];
    CVECTOR color[8];
    CVECTOR burst_color[8];
} Model835State;

extern GsIMAGE D_8013C810[];
extern Model835Config D_8013C848[];

s32 func_8013B004(u8 *context, s32 command);

#endif

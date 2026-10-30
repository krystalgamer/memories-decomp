#ifndef MEMORIES_DECOMP_MODEL_VARIANT836_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT836_ENTRY_H

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

/* One 0x1A-byte command row: the orb colour, the burst colour, the model
 * part each side's orbs leave from, the orb count, the target spread, the
 * flight length, the spark count and spread, the launch and stop times and
 * the two camera times. */
typedef struct {
    u8 r, g, b;
    u8 burst_r, burst_g, burst_b;
    u8 part[2];
    u16 max_count;
    u16 spread;
    u16 steps;
    u16 spark_max;
    u16 spark_spread;
    u16 start;
    u16 stop;
    u16 camera_times[2];
} Model836Config;

/* The work area: the command row; per orb and side its position, step,
 * ten-point trail, burst position, spark frames, state, drawn trail length,
 * spark count, trail head and alpha; per orb its four spark offsets, burst
 * scale and orb and burst colours; the timers, phase, orb count, ambient
 * level, the burst texture and the two loaded textures. */
typedef struct {
    Model836Config *G32 config;
    SVECTOR pos[40][2];
    SVECTOR vel[40][2];
    SVECTOR trail[40][10][2];
    SVECTOR sparks[40][4];
    SVECTOR burst_pos[40][2];
    u32 elapsed;
    u32 tick;
    u32 scale[40];
    u16 spark_frame[40][4][2];
    u16 phase;
    u16 state[40][2];
    u16 count;
    u16 trail_count[40][2];
    u16 spark_count[40][2];
    u16 trail_head[40][2];
    s32 level;
    u16 burst_tpage;
    u16 burst_clut;
    s32 texture[2];
    u8 alpha[40][2];
    CVECTOR color[40];
    CVECTOR burst_color[40];
} Model836State;

extern GsIMAGE D_8013CAD0[];
extern Model836Config D_8013CB08[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT797_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT797_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1E-byte command row: the model part the twin flares sit beside, the
 * burst colour, the strike and burst sizes, the ray spread and length, the
 * burst period, the fade step, the hold, emission and camera times and the
 * start delay. */
typedef struct {
    u8 part, unknown_01;
    u8 r, unknown_03;
    u8 g, unknown_05;
    u8 b, unknown_07;
    u16 strike_size;
    u16 burst_size;
    u16 spread;
    u16 ray_size;
    u16 burst_period;
    u8 fade, unknown_13;
    u16 unknown_14;
    u16 hold;
    u16 emit_end;
    u16 camera_time;
    u16 start;
} Model797Config;

/* The work area: the strike and burst positions, sixteen ray tips per burst,
 * the two flare offsets, the spins of the flare rays, the strike frames,
 * the timers, the ambient level, the counts and phase, the textures, the
 * fading burst colours and the four flare colour levels. */
typedef struct {
    Model797Config *G32 config;
    SVECTOR strikes[64];
    SVECTOR bursts[64];
    SVECTOR rays[64][16];
    SVECTOR offsets[2];
    SVECTOR spins[2][16];
    u16 strike_frames[64];
    u16 elapsed;
    u16 tick;
    s32 level;
    u16 strike_count;
    u16 phase;
    u16 burst_count;
    s32 texture[2];
    u8 burst_r[64];
    u8 burst_g[64];
    u8 burst_b[64];
    u8 flare[4];
} Model797State;

extern GsIMAGE D_8013C6D0[2];
extern Model797Config D_8013C708[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT774_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT774_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x38-byte command row: the model point the halo sits on, the blade
 * shape, the flare size, the blade spin, the blade count, the ring radius,
 * the target spread, the flight length and swing, the impact glow size and
 * growth, its rays, the two impact ring radii, the halo size and the launch
 * period, start and time. */
typedef struct {
    u8 r, g, b;
    u8 part;
    s16 height;
    s16 front;
    s16 size;
    s16 back;
    s16 spin;
    s16 count;
    s16 radius;
    s16 spread;
    s16 steps;
    s16 swing;
    s16 glow_size;
    s16 unknown_1A;
    s32 grow;
    s32 fade_grow;
    s16 ray_spread;
    s16 ray_width;
    s16 ray_count;
    s16 inner_radius;
    s16 outer_radius;
    s16 halo_size;
    s16 period;
    s16 start;
    s16 launch;
    s16 unknown_36;
} Model774Config;

/* The work area: the command row, the four blade corners; per blade its
 * position, target, step, angle, origin, thirty-two-point trail, impact
 * rays and two impact rings; the textures, the trail heads and flight
 * frames, the launched count, the phase, the per-blade states, the timers,
 * the ambient level, the impact scales, the halo level and the per-blade
 * trail and impact levels. */
typedef struct {
    Model774Config *G32 config;
    SVECTOR blade[4];
    SVECTOR pos[8];
    SVECTOR target[8];
    SVECTOR step[8];
    VECTOR angle[8];
    SVECTOR origin[8];
    SVECTOR trail[8][32];
    SVECTOR rays[8][32];
    SVECTOR inner[8][16];
    SVECTOR outer[8][16];
    s32 texture[3];
    s16 head[8];
    s16 frame[8];
    s16 active;
    s16 phase;
    s16 state[8];
    s16 elapsed;
    s16 frames;
    s32 ambient;
    s32 scale[8];
    u8 level;
    u8 trail_level[8];
    u8 glow[8];
} Model774State;

extern GsIMAGE D_8013E384[];
extern Model774Config D_8013E3D8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

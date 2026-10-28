#ifndef MEMORIES_DECOMP_MODEL_VARIANT781_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT781_ENTRY_H

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

/* One 0x34-byte command row: the orb colour, the two model parts the orbs
 * leave from, the flight steps, the orb, glow, ray height and orbit sizes,
 * the two ring radii and the ring depth, the halo and debris sizes, the
 * debris speed, the fade step, the colour step, the two halo growth rates,
 * the four launch deadlines, the start time and the two camera times. */
typedef struct {
    u8 r, g, b;
    u8 part[2];
    u8 unknown_05;
    u16 flight;
    u16 size;
    u16 glow_size;
    u16 height;
    u16 orbit;
    u16 ring_outer;
    u16 ring_inner;
    u16 ring_depth;
    u16 halo_size;
    u16 debris_size;
    u16 debris_speed;
    u8 fade, unknown_1D;
    u16 unknown_1E;
    u8 color_step, unknown_21;
    u16 grow;
    u16 slow_grow;
    s16 time[2][2];
    s16 start;
    s16 orbit_end;
    s16 pan_end;
} Model781Config;

/* The work area, one entry per model part and orb: the orb positions and
 * velocities, sixteen orbiting sparks per orb, the sixteen-step halo ring,
 * thirty-two debris pieces per orb with their velocities, the timer, the
 * spin angles, the ray heights, the phase, the arrival states, the halo
 * scales, the ambient level, the three textures and the orb colours. */
typedef struct {
    Model781Config *G32 config;
    SVECTOR pos[2][2];
    SVECTOR velocity[2][2];
    SVECTOR sparks[2][2][16];
    SVECTOR ring[16][2];
    SVECTOR debris[2][2][32];
    SVECTOR debris_velocity[2][2][32];
    u32 elapsed;
    s16 angle[2][2];
    u16 height[2][2];
    u16 phase;
    u16 arrived[2][2];
    u32 scale[2][2];
    s32 ambient;
    s32 texture[3];
    u8 r[2][2];
    u8 g[2][2];
    u8 b[2][2];
} Model781State;

extern GsIMAGE D_8013CDD4[];
extern Model781Config D_8013CE28[];

s32 func_8013B004(u8 *context, s32 command);

#endif

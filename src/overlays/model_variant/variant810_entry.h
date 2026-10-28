#ifndef MEMORIES_DECOMP_MODEL_VARIANT810_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT810_ENTRY_H

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

/* One 0x42-byte command row: the ring and flare colours, the two model
 * points per side, the radii of the four inner/outer ring fans, their
 * depths and spin, the halo size, the ribbon width, the smoke puff size,
 * speed and count, the burst fan and flare sizes, the fade steps and the
 * per-side start and end times. */
typedef struct {
    u8 r, g, b;
    u8 flare_r, flare_g, flare_b;
    u8 part[2];
    u8 puff_part[2];
    u16 inner_radius;
    u16 inner_outer;
    u16 outer_radius;
    u16 outer_outer;
    u16 outer_depth;
    u16 spin;
    u16 inner_depth;
    u16 halo_size;
    u16 ribbon_width;
    u16 halo_radius;
    u16 halo_outer;
    u16 halo2_radius;
    u16 halo2_outer;
    u16 halo_depth;
    u16 puff_size;
    u16 puff_speed;
    u16 puff_count;
    u16 burst_radius;
    u16 burst_outer;
    u16 burst_depth;
    u16 flare_size;
    u8 fade, unknown_35;
    u16 unknown_36;
    u8 ring_fade, unknown_39;
    u16 start[2];
    u16 end[2];
} Model810Config;

/* The work area: the command row; the sixteen-segment ring fans; the
 * smoke puffs and their velocities per side; the burst fan; the 32-point
 * ribbon trail per side; the timer, the phase, the puff frames, the
 * ribbon tails and heads, the ring and burst scales, the ambient level,
 * the textures and the per-side ring, puff-ring and flare colours. */
typedef struct {
    Model810Config *G32 config;
    SVECTOR ring[16][2];
    SVECTOR ring_outer[16][2];
    SVECTOR halo[16][2];
    SVECTOR halo_outer[16][2];
    SVECTOR puffs[2][16];
    SVECTOR puff_velocity[2][16];
    SVECTOR burst[16][2];
    SVECTOR trail[2][32];
    u32 elapsed;
    u16 phase;
    u16 puff_frame[2][16];
    u16 tail[2];
    u16 head[2];
    u16 unknown_952;
    s32 ring_scale[2];
    s32 burst_scale[2];
    s32 ambient;
    s32 texture[4];
    u8 ring_r[2];
    u8 ring_g[2];
    u8 ring_b[2];
    u8 halo_r[2];
    u8 halo_g[2];
    u8 halo_b[2];
    u8 flare_r[2];
    u8 flare_g[2];
    u8 flare_b[2];
} Model810State;

extern GsIMAGE D_8013DA80[];
extern Model810Config D_8013DAF0[];

s32 func_8013B004(u8 *context, s32 command);

#endif

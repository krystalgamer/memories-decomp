#ifndef MEMORIES_DECOMP_MODEL_VARIANT813_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT813_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x2A-byte command row: the orb, the comet and the burst colours, the
 * two model parts the effect leaves from, the orb spread, the orb, comet,
 * burst and shard sizes, the flight time, the shard speed, the orb and comet
 * periods, the two fade steps, and the timing. */
typedef struct {
    u8 orb_r, orb_g, orb_b;
    u8 comet_r, comet_g, comet_b;
    u8 burst_r, burst_g, burst_b;
    u8 parts[2];
    u8 unknown_B;
    u16 spread;
    u16 orb_size;
    u16 comet_size;
    u16 flight;
    u16 orb_burst_size;
    u16 comet_burst_size;
    u16 shard_size;
    u16 shard_speed;
    u16 orb_period;
    u16 comet_period;
    u8 orb_fade, unknown_21;
    u8 comet_fade, unknown_23;
    u16 unknown_24;
    u16 start;
    u16 emit_end;
} Model813Config;

/* The work area, one half per model part: the orbs with their velocities,
 * the comets with their velocities, eight shards per comet with their
 * velocities, the timers, the spins, the target depths, the phase, the
 * arrival flags, the counts, the ambient level, the textures and the fading
 * burst colours. */
typedef struct {
    Model813Config *G32 config;
    SVECTOR orbs[2][64];
    SVECTOR orb_velocities[2][64];
    SVECTOR comets[2][32];
    SVECTOR comet_velocities[2][32];
    SVECTOR shards[2][32][8];
    SVECTOR shard_velocities[2][32][8];
    u32 elapsed;
    u32 tick;
    u16 orb_spins[2][64];
    u16 comet_spins[2][32];
    s16 orb_targets[2][64];
    s16 comet_targets[2][32];
    u16 phase;
    u16 orb_arrived[2][64];
    u16 comet_arrived[2][32];
    u16 orb_count;
    u16 comet_count;
    s32 level;
    s32 texture[5];
    u8 orb_r[2][64];
    u8 orb_g[2][64];
    u8 orb_b[2][64];
    u8 comet_r[2][32];
    u8 comet_g[2][32];
    u8 comet_b[2][32];
} Model813State;

extern GsIMAGE D_8013CE80[5];
extern Model813Config D_8013CF0C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

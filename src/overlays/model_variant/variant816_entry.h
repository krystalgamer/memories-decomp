#ifndef MEMORIES_DECOMP_MODEL_VARIANT816_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT816_ENTRY_H

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

/* One 0x3A-byte command row: the orb, the comet and the burst colours, the
 * model part the effect leaves from, the spread, the orb and comet sizes,
 * the flight time, the orb and comet burst sizes, the shard size and speed,
 * the spinner size and spin, the debris size, spread and fall, the three
 * periods, the three fade steps, and the timing. */
typedef struct {
    u8 orb_r, orb_g, orb_b;
    u8 comet_r, comet_g, comet_b;
    u8 burst_r, burst_g, burst_b;
    u8 parts[1];
    u16 spread;
    u16 orb_size;
    u16 comet_size;
    u16 flight;
    u16 orb_burst_size;
    u16 comet_burst_size;
    u16 shard_size;
    u16 shard_speed;
    u16 spinner_width;
    u16 spinner_height;
    u16 spin;
    u16 debris_width;
    u16 debris_height;
    u16 debris_spread;
    u16 debris_fall;
    u16 orb_period;
    u16 comet_period;
    u16 spinner_period;
    u8 orb_fade, unknown_2F;
    u8 comet_fade, unknown_31;
    u16 unknown_32;
    u8 spinner_fade, unknown_35;
    u16 start;
    u16 emit_end;
} Model816Config;

/* The work area: the orbs with their velocities and spins, the comets with
 * their velocities, spins, eight shards each and the shard velocities, the
 * spinners with their velocities and spins, eight debris pieces each with
 * their velocities and spins, the timers, the target depths, the phase, the
 * arrival flags, the counts, the ambient level, the textures, the fading
 * burst colours and the spinner debris brightness. */
typedef struct {
    Model816Config *G32 config;
    SVECTOR orbs[1][64];
    SVECTOR orb_velocities[1][64];
    SVECTOR orb_spins[1][64];
    SVECTOR comets[1][16];
    SVECTOR comet_velocities[1][16];
    SVECTOR comet_spins[1][16];
    SVECTOR shards[1][16][8];
    SVECTOR shard_velocities[1][16][8];
    SVECTOR spinners[1][32];
    SVECTOR spinner_velocities[1][32];
    SVECTOR spinner_spins[1][32];
    SVECTOR debris[1][32][8];
    SVECTOR debris_velocities[1][32][8];
    SVECTOR debris_spins[1][32][8];
    u32 elapsed;
    u32 tick;
    s16 orb_targets[1][64];
    s16 comet_targets[1][16];
    s16 spinner_targets[1][32];
    u16 phase;
    u16 orb_arrived[1][64];
    u16 comet_arrived[1][16];
    u16 spinner_arrived[1][32];
    u16 orb_count;
    u16 comet_count;
    u16 spinner_count;
    s32 level;
    s32 texture[5];
    u8 orb_r[1][64];
    u8 orb_g[1][64];
    u8 orb_b[1][64];
    u8 comet_r[1][16];
    u8 comet_g[1][16];
    u8 comet_b[1][16];
    u8 debris_level[1][32];
} Model816State;

extern GsIMAGE gModelVariant816_D2B7C[5];
extern Model816Config gModelVariant816_D2C08[];

s32 ModelVariant816_HandleRequest(u8 *context, s32 command);

#endif

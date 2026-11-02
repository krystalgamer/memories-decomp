#ifndef MEMORIES_DECOMP_MODEL_VARIANT832_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT832_ENTRY_H

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
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x58-byte command row: the muzzle sprite colour, the bolt, glow and
 * flare colours, the model point the bolts leave from, the muzzle size, the
 * flight length, the bolt count, the ring of sprites and spinning strips
 * around each bolt, the smoke puffs, the impact glow, the shock fan, the
 * sparks, the fade step, the fan growth rates, the four launch times and
 * the camera cue times. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 unknown_09[3];
    u8 part;
    u8 unknown_0D[3];
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 unknown_28;
    u16 fan_radius;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_37;
    u32 unknown_38;
    u32 grow;
    u32 grow_late;
    u16 start;
    u16 launch[3];
    u16 unknown_4C[2];
    u16 cue[3];
    u16 unknown_56;
} Model832Config;

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, sixteen-point trail and origin; the impact
 * embers and their velocities; the smoke puffs and their velocities; the
 * flare sprites and their velocities; the sixteen inner and outer fan
 * points; the sparks; the glow motes and their velocities; the shock
 * debris and its velocities; the burst shards and their velocities; the spinning blade angles; then the timer,
 * the flight counters, the fan growth, the blade stretch, the puff and
 * spark frames, the debris frames, the trail lengths, the spark counts,
 * the trail heads, the launched bolt count, the arrival flags, the phase,
 * the ambient level, the shared texture page, the textures and the
 * per-bolt bolt, glow and flare colours. */
typedef struct {
    Model832Config *G32 config;
    SVECTOR pos[4];
    SVECTOR step[4];
    SVECTOR angle[4];
    SVECTOR ring[4][16];
    SVECTOR target[4];
    SVECTOR trail[4][16];
    SVECTOR origin[4];
    SVECTOR embers[4][8];
    SVECTOR ember_velocity[4][8];
    SVECTOR puffs[4][8];
    SVECTOR puff_velocity[4][8];
    SVECTOR flares[4][16];
    SVECTOR flare_velocity[4][16];
    SVECTOR fan[16][2];
    SVECTOR sparks[4][48];
    SVECTOR motes[4][16];
    SVECTOR mote_velocity[4][16];
    SVECTOR debris[4][16];
    SVECTOR debris_velocity[4][16];
    SVECTOR shards[4][64];
    SVECTOR shard_velocity[4][64];
    SVECTOR blades[4][32];
    u16 elapsed;
    u16 flight[4];
    u16 growth[4];
    u16 unknown_2BB6[4];
    u16 stretch[4];
    u16 puff_frame[4][8];
    u16 spark_frame[4][48];
    u16 debris_frame[4];
    u16 tail[4];
    u16 unknown_2D96;
    u16 spark_count[4];
    u16 trail_head[4];
    u16 launched;
    u16 arrived[4];
    u16 phase;
    s32 ambient;
    u16 tpage;
    u16 clut;
    s32 texture[12];
    u8 bolt[4][4];
    u8 glow[4][4];
    u8 flare[4][4];
} Model832State;

extern GsIMAGE gModelVariant832_D4D4C[12];
extern Model832Config gModelVariant832_D4E9C[];

s32 ModelVariant832_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT764_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT764_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x20-byte command row: the colour, the inner and outer ring radii, the
 * ring growth before and after the threshold, the cone height, the shard
 * count, spread and speed, the half length of the flash quad, the vertical
 * spread, the start delay and the end of the effect. */
typedef struct {
    u8 r, g, b, unknown_03;
    s16 inner_radius;
    s16 outer_radius;
    s32 grow;
    s32 grow_late;
    s16 cone_height;
    s16 shard_count;
    s16 shard_spread;
    s16 shard_speed;
    s16 quad_half;
    s16 shard_spread_y;
    s16 delay;
    s16 end;
} Model764Config;

/* The 0x3B8-byte work area: the command row, the inner, outer and cone
 * rings, the shards, the flash quad, the shard velocities, the texture, the
 * ring scale, the flash frame, the elapsed time, the ambient level, the phase,
 * the ring colour and the fading grey. */
typedef struct {
    Model764Config *G32 config;
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR cone[16];
    SVECTOR shards[32];
    SVECTOR quad[4];
    SVECTOR velocities[32];
    s32 texture[1];
    s32 scale;
    s16 flash;
    s16 elapsed;
    s16 ambient;
    s16 phase;
    u8 color[3];
    u8 grey;
} Model764State;

extern GsIMAGE D_8013C2FC[1];
extern Model764Config D_8013C318[];

s32 func_8013B004(u8 *context, s32 command);

#endif

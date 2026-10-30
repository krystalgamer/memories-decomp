#ifndef MEMORIES_DECOMP_MODEL_VARIANT826_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT826_ENTRY_H

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
#include "../../game/graphics_constants.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1C-byte command row: the screen tint colour, the colour the shards
 * flare to, the model part the glow follows, the glow and shard half-sizes,
 * the shard fade step, the glow, tint and shard flare steps, the delay, and
 * the start and the end of shard spawning. */
typedef struct {
    u8 r, g, b;
    u8 shard_r, shard_g, shard_b;
    u8 part, pad_7;
    s16 glow_size;
    s16 unknown_a;
    u16 shard_size;
    u16 shard_fade;
    u16 glow_step;
    u16 tint_step;
    u16 shard_step;
    u16 delay;
    u16 start;
    u16 end;
} Model826Config;

/* The screen tint colour and the glow brightness, which the entry also tests
 * as one word to know whether any tint is left. */
typedef union {
    struct {
        u8 r, g, b, glow;
    } c;
    u32 word;
} Model826Color;

/* The work area: the command row, the shard positions, the elapsed time, the
 * frame counter, the phase, the glow pulse direction, the shard flare flags,
 * the shard count, the glow scale, the ambient level, the textures, the tint
 * and glow, and each shard's colour channels. */
typedef struct {
    Model826Config *G32 config;
    SVECTOR positions[16];
    u32 elapsed;
    u32 tick;
    u16 phase;
    u16 pulse;
    u16 flared[16];
    u16 count;
    u32 scale;
    s32 ambient;
    s32 texture[2];
    Model826Color color;
    u8 r[16];
    u8 g[16];
    u8 b[16];
} Model826State;

extern GsIMAGE D_8013D218[2];
extern Model826Config D_8013D250[];

s32 func_8013B004(u8 *context, s32 command);

#endif

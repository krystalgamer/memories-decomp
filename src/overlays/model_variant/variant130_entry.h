#ifndef MEMORIES_DECOMP_MODEL_VARIANT130_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT130_ENTRY_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x16-byte command row: the sprite colour and model part, the sprite
 * count and trail length, the sprite size, the target spread, the depth that
 * starts the trail, the flight, growth and spawn timings, the spin speed and
 * the start delay. */
typedef struct {
    u8 red, green, blue, part;
    u8 count, trail;
    u16 size;
    s16 spread, trail_depth, duration, grow, interval, spin, delay;
} Model130Config;

/* The work area: up to 64 sprites flying from the model part to a random
 * target, the texture, the finished-sprite counter and the timer. */
typedef struct {
    Model130Config *G32 config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    u32 texture[1];
    s32 unknown_408;
    u8 done;
    s32 elapsed;
} Model130State;

extern GsIMAGE gModelVariant130_D0B6C[1];
extern Model130Config gModelVariant130_D0B88[];

s32 ModelVariant130_HandleRequest(u8 *context, s32 command);

#endif

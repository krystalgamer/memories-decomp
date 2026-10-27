#ifndef MEMORIES_DECOMP_MODEL_VARIANT117_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT117_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x20-byte command row: the colours of the sprites, the beam and the
 * spinning squares, the attachment part, the sizes, and the phase timings. */
typedef struct {
    u8 sprite_red, sprite_green, sprite_blue;
    u8 beam_red, beam_green, beam_blue;
    u8 square_red, square_green, square_blue;
    u8 part;
    s16 sprite_size, spread, beam_radius, square_size, unknown_12;
    s16 sprite_period, rise_duration, spin_period, start_delay, delay, duration;
} Model117Config;

/* The work area: eight sprite directions, the nine-point beam ring, two
 * four-vertex squares, the per-sprite timers, five texture words and the
 * elapsed time. */
typedef struct {
    Model117Config *G32 config;
    SVECTOR directions[8];
    SVECTOR ring[9];
    SVECTOR squares[8];
    s16 timers[8];
    u32 texture[5];
    s32 elapsed;
} Model117State;

extern GsIMAGE D_8013CC00[];
extern Model117Config D_8013CC8C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

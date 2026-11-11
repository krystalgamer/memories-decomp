#ifndef MEMORIES_DECOMP_MODEL_VARIANT117_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT117_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
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
    Model117Config *config;
    SVECTOR directions[8];
    SVECTOR ring[9];
    SVECTOR squares[8];
    s16 timers[8];
    u32 texture[5];
    s32 elapsed;
} Model117State;

extern GsIMAGE gModelVariant117_aImages[];
extern Model117Config gModelVariant117_aConfigs[];

s32 ModelVariant117_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT103_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT103_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x1E-byte command row: the fan, blade and strip colours, the slot
 * part the effect follows, the fan and arc radius, the blade length and
 * width, the fan's grow, hold and fade times, the strips' spread and fade
 * times (both counted from the end of the hold) and the start delay. */
typedef struct {
    u8 red, green, blue;
    u8 blade_red, blade_green, blade_blue;
    u8 strip_red, strip_green, strip_blue;
    u8 part;
    s16 radius, blade_length, blade_width, unknown_10;
    s16 grow_duration, hold_duration, fade_duration, strip_duration, strip_fade_duration;
    s16 delay;
} Model103Config;

/* The 0xE4-byte work area: the command row, the fan's centre and rim, the
 * last followed position, the strip arc, the texture and the elapsed time. */
typedef struct {
    Model103Config *G32 config;
    SVECTOR fan[18];
    SVECTOR position;
    SVECTOR ring[8];
    u32 texture[1];
    s32 elapsed;
} Model103State;

extern GsIMAGE gModelVariant103_D1244[];
extern Model103Config gModelVariant103_D1260[];

s32 ModelVariant103_HandleRequest(u8 *context, s32 command);

#endif

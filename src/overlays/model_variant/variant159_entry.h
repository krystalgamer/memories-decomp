#ifndef MEMORIES_DECOMP_MODEL_VARIANT159_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT159_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"

/* One 0x1E-byte command row, picked by command % 100: the start, middle and
 * end colours of the sprites, an unused byte, the curtain half width, height
 * and wave depth, the sprite size, the rise, hold and fall durations of the
 * curtain, the sprite duration, the wave speed and the start delay. */
typedef struct {
    u8 red, green, blue, mid_red, mid_green, mid_blue, end_red, end_green, end_blue, unknown_9;
    s16 width, height, wave, size, rise, hold, fall, sprite, speed, delay;
} Model159Config;

/* The 0x234-byte work area: the command row, a 17-point curtain edge, eight
 * sprite positions, five rows of 17 curtain colours, the texture word, the
 * completion flag, the curtain depth, the elapsed time and a frame counter. */
typedef struct {
    Model159Config *G32 config;
    SVECTOR edge[17];
    SVECTOR sprites[8];
    CVECTOR colors[5][17];
    u32 texture[1];
    u8 unknown_224[4];
    u8 completed, unknown_229;
    s16 depth;
    s32 elapsed;
    s32 frames;
} Model159State;

extern GsIMAGE gModelVariant159_D1D94[];
extern Model159Config gModelVariant159_D1DB0[];

s32 ModelVariant159_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT119_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT119_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_effect_requests.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../game/screen_projection.h"

/* One 0x1A-byte command row: the blade and flash colours, the camera part
 * (0xFF for a fixed point), the number of waves, the blade shape, the slot
 * animation window, and the fade timing. */
typedef struct {
    u8 red, green, blue, flash_red, flash_green, flash_blue, part, count;
    u16 half_width;
    s16 depth;
    u16 length, anim_span;
    s16 fade, spacing, anim_start, delay, start;
} Model119Config;

/* The work area: the command row, the four blade vertices, the camera step
 * counter, the texture word, the elapsed time and the tint flag. */
typedef struct {
    Model119Config *G32 config;
    SVECTOR vertices[4];
    u8 camera_steps, unknown_25[3];
    s32 texture[1];
    u8 unknown_2C[4];
    s32 elapsed;
    u8 started, unknown_35[3];
    s32 tint;
} Model119State;

extern const VECTOR D_8013BA1C;
extern GsIMAGE D_8013BA2C[];
extern Model119Config D_8013BA48[];

s32 func_8013B004(u8 *context, s32 command);

#endif

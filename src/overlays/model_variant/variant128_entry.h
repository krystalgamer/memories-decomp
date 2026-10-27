#ifndef MEMORIES_DECOMP_MODEL_VARIANT128_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT128_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x56-byte command row: the screen flash, dot and trail colours, the
 * slot parts the trails follow (-1 terminated, also the tinted parts), the
 * dot shell radius, the trail half height, the flash, dot and trail
 * durations, up to eight tint delays, windows and durations (-1 terminated)
 * and the start delay. */
typedef struct {
    u8 flash_red, flash_green, flash_blue, box_red, box_green, box_blue;
    u8 trail_red, trail_green, trail_blue, unknown_09;
    s16 parts[8];
    s16 radius, height;
    s16 flash_duration, box_duration, trail_duration;
    s16 tint_delay[8], tint_window[8], tint_duration[8];
    s16 delay;
} Model128Config;

/* One trail: the part position latched during the first frames after the
 * trails start and the part position followed until they start. */
typedef struct {
    SVECTOR head;
    SVECTOR tail;
} Model128Trail;

/* The 0x2D0-byte work area: the command row, eight trails, a shell of 64
 * random dots, one texture, the hit flag, the long-command flag, the elapsed
 * time and a latch counter for each trail. */
typedef struct {
    Model128Config *G32 config;
    Model128Trail trails[8];
    SVECTOR dots[64];
    u32 texture[1];
    s32 elapsed;
    u8 hit, tinted;
    u8 latch[64];
} Model128State;

extern GsIMAGE D_8013C5A8[];
extern Model128Config D_8013C5C4[];

s32 func_8013B004(u8 *context, s32 command);

#endif

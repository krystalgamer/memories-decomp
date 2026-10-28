#ifndef MEMORIES_DECOMP_MODEL_VARIANT120_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT120_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
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
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x1C-byte command row: the sprite, blade and flash colours, the camera
 * part, the sprite half size, the blade width and height, the slot animation
 * window, the fade and ambient-recovery times, the delay and the start. */
typedef struct {
    u8 red, green, blue, blade_red, blade_green, blade_blue;
    u8 flash_red, flash_green, flash_blue, part;
    u16 half_size;
    s16 width, height;
    u16 anim_span;
    s16 duration, recovery, anim_start, delay, start;
} Model120Config;

/* The work area: the command row, each blade's start angle and spin speed,
 * the camera step counter, the texture word, the elapsed time and the tint
 * flag. */
typedef struct {
    Model120Config *G32 config;
    SVECTOR angles[16];
    SVECTOR speeds[16];
    u8 camera_steps, unknown_105[3];
    s32 texture[1];
    s32 elapsed;
    u8 started, unknown_111[3];
    s32 tint;
} Model120State;

extern GsIMAGE D_8013C274[];
extern Model120Config D_8013C290[];

s32 func_8013B004(u8 *context, s32 command);

#endif

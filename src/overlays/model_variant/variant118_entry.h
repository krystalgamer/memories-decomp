#ifndef MEMORIES_DECOMP_MODEL_VARIANT118_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT118_ENTRY_H

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
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x20-byte command row: the trail, fan and dot colours, the camera part,
 * the shapes (quad half size, fan height and width, dot shell radius), the
 * slot animation window, and the three phase durations and their timing. */
typedef struct {
    u8 quad_red, quad_green, quad_blue, fan_red, fan_green, fan_blue;
    u8 box_red, box_green, box_blue, part;
    u16 half_size;
    s16 height;
    s16 width;
    s16 radius;
    u16 anim_span;
    s16 quad_duration, fan_duration, box_duration, anim_start, delay, start;
} Model118Config;

/* The work area: the command row, the last three positions of the camera
 * part, the 64 dots of the shell and their fan scales, the camera step
 * counter, the two texture words, the elapsed time and the tint flag. */
typedef struct {
    Model118Config *G32 config;
    SVECTOR trail[3];
    SVECTOR dots[64];
    s32 scales[64];
    u8 camera_steps, unknown_31D[3];
    s32 texture[2];
    s32 elapsed;
    u8 started, unknown_32D[3];
    s32 tint;
} Model118State;

extern const VECTOR D_8013C044;
extern GsIMAGE D_8013C054[];
extern Model118Config D_8013C08C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

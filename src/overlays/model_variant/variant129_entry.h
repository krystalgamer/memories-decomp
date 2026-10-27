#ifndef MEMORIES_DECOMP_MODEL_VARIANT129_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT129_ENTRY_H

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

/* One 0x46-byte command row, picked by command % 100: the fan and dot
 * colours, the slot part each of up to sixteen points starts from, the fan
 * height and width, the dot shell radius, the slot animation window, the fan
 * and dot durations, the animation start, the delay and the start times of
 * the points, ended by a negative one. */
typedef struct {
    u8 red, green, blue, box_red, box_green, box_blue;
    u8 parts[16];
    s16 height;
    s16 width;
    s16 radius;
    u16 anim_span;
    s16 fan_duration, box_duration, anim_start, delay;
    s16 starts[16];
} Model129Config;

/* The work area: the command row, the sixteen point positions, the 64 dots
 * of the shell and their fan scales, the camera step counter, the two
 * texture words, the elapsed time, the number of points started and the tint
 * flag. */
typedef struct {
    Model129Config *G32 config;
    SVECTOR positions[16];
    SVECTOR dots[64];
    s32 scales[64];
    u8 camera_steps, unknown_385[3];
    s32 texture[2];
    s32 elapsed;
    u8 started, unknown_395[3];
    s32 tint;
} Model129State;

extern const VECTOR D_8013BFA0;
extern GsIMAGE D_8013BFB0[];
extern Model129Config D_8013BFE8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT761_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT761_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x1C-byte command row: the colour, the beam half width and height, the
 * beam's rising speed, the ring's depth, the cone height, its offset and
 * length, the start delay, the burst and end times, and the window during
 * which the slot's parts are tinted. */
typedef struct {
    u8 r, g, b, pad_3;
    s16 beam_width;
    u16 beam_height;
    s16 rise;
    u16 ring_depth;
    u16 cone_height;
    s16 cone_offset;
    s16 cone_length;
    s16 delay;
    s16 burst_time;
    s16 end_time;
    s16 tint_start;
    s16 tint_stop;
} Model761Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model761Color;

/* The work area: the command row, the beam quad and its offset, the two ring
 * edges, the cone quad, the beam's vertical scale, the ambient level, the
 * textures, the timers, the ring frame and the beam colour. */
typedef struct {
    Model761Config *G32 config;
    SVECTOR beam[4];
    SVECTOR offset;
    SVECTOR inner[9];
    SVECTOR outer[9];
    SVECTOR cone[4];
    u8 unknown_DC[8];
    u32 beam_scale;
    s32 level;
    s32 texture[3];
    s16 phase;
    s16 elapsed;
    u16 tick;
    s16 frame;
    Model761Color color;
} Model761State;

extern GsIMAGE D_8013C1F0[];
extern Model761Config D_8013C244[];

s32 func_8013B004(u8 *context, s32 command);

#endif

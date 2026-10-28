#ifndef MEMORIES_DECOMP_MODEL_VARIANT95_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT95_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x1C-byte command row: the sphere colour, the beam colour, the slot
 * part the sphere follows during the lead-in, the sphere radius, the beam
 * travel, the sphere grow time, the beam lifetime and its fade start, the
 * flash duration, the lead-in time and the delay before the sphere draws. */
typedef struct {
    u8 red, green, blue;
    u8 beam_red, beam_green, beam_blue;
    u8 part, unknown_07;
    s16 radius, unknown_0A, travel, unknown_0E, grow, life, fade, flash, start, delay;
} Model95Config;

/* The 0x10AC-byte work area: the command row, a 497-point sphere (two poles
 * around 15 rings of 33 points), a 33-point beam ring, the sphere position,
 * two texture words, the completion flag and the elapsed time. */
typedef struct {
    Model95Config *G32 config;
    SVECTOR points[497];
    SVECTOR ring[33];
    SVECTOR position;
    s32 texture[2];
    u8 completed, unknown_10A5[3];
    s32 elapsed;
} Model95State;

extern GsIMAGE D_8013C3F4[];
extern Model95Config D_8013C42C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT154_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT154_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/screen_projection.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"

/* One 0x22-byte command row, picked by command % 100: the trail, ring, beam
 * and burst colours, the slot part the effect starts from, the grid half
 * size, the ring radius and the burst ring's extra radius and depth, the
 * cycle length, the flash and burst durations, the cycle count, the start
 * time and the flash start time. */
typedef struct {
    u8 red, green, blue;
    u8 ring_red, ring_green, ring_blue;
    u8 beam_red, beam_green, beam_blue;
    u8 burst_red, burst_green, burst_blue;
    u8 part, unknown_D;
    s16 size, radius, spread, depth, period, flash, burst, count, delay, flash_delay;
} Model154Config;

/* The 0x4C0-byte work area: the command row, an 8-entry trail ring buffer,
 * a 5 x 5 grid mesh, a 17-point ring, a 66-point double ring, the corner
 * pointers of the grid's 16 quads, the burst heading, three texture words,
 * the completion flag, the trail head, the elapsed time and a frame
 * count. */
typedef struct {
    Model154Config *G32 config;
    SVECTOR trail[8];
    SVECTOR grid[25];
    SVECTOR ring[17];
    SVECTOR rings[66];
    SVECTOR *G32 quads[16][4];
    s16 heading, unknown_4A6;
    u32 texture[3];
    u8 completed, head, unknown_4B6[2];
    s32 elapsed;
    s32 frames;
} Model154State;

extern GsIMAGE D_8013D3B4[];
extern Model154Config D_8013D408[];

s32 func_8013B004(u8 *context, s32 command);

#endif

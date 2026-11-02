#ifndef MEMORIES_DECOMP_MODEL_VARIANT169_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT169_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"

/* One 0x22-byte command row: the trail, sprite, beam and ring colours, the
 * slot part the effect starts from, the curved grid half size, the beam ring
 * radius, the halo ring width and depth, the per-burst travel period, the
 * flash and halo ring durations, the burst count and the effect and flash
 * delays. */
typedef struct {
    u8 trail_red, trail_green, trail_blue;
    u8 sprite_red, sprite_green, sprite_blue;
    u8 beam_red, beam_green, beam_blue;
    u8 ring_red, ring_green, ring_blue;
    u8 part, unknown_0D;
    s16 grid_size, radius, ring_width, depth;
    s16 period, flash_duration, ring_duration, count, delay, flash_delay;
} Model169Config;

/* The work area: eight trail positions (the pad word marks a travelling
 * entry), the curved 5x5 grid and its sixteen quads, the 17-point beam ring,
 * the two 33-point halo rings, the halo angle, the three texture words, the
 * completion flag, the trail cursor, the elapsed time and a frame counter. */
typedef struct {
    Model169Config *G32 config;
    SVECTOR trail[8];
    SVECTOR grid[25];
    SVECTOR beam_ring[17];
    SVECTOR rings[66];
    SVECTOR *G32 quads[16][4];
    s16 angle, unknown_4A6;
    u32 texture[3];
    u8 completed, cursor, unknown_4B6[2];
    s32 elapsed;
    s32 frame;
} Model169State;

extern GsIMAGE gModelVariant169_D20E4[];
extern Model169Config gModelVariant169_D2138[];

s32 ModelVariant169_HandleRequest(u8 *context, s32 command);

#endif

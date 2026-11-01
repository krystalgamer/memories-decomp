#ifndef MEMORIES_DECOMP_MODEL_VARIANT144_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT144_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2E-byte command row: the colours of the rings, the screen flash,
 * the sparks, the grid sprite and the trails, the part the effect follows,
 * the five parts the trails pass through, the ring radii and depth, the
 * spark radius, the grid spacing, the phase durations and the three start
 * times. */
typedef struct {
    u8 ring_red, ring_green, ring_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 grid_red, grid_green, grid_blue;
    u8 trail_red, trail_green, trail_blue;
    u8 part;
    u8 trail_parts[5];
    s16 outer_radius, inner_radius, ring_depth, spark_radius, grid_spacing;
    s16 ring_duration, flash_duration, spark_duration, trail_duration;
    s16 ring_delay, burst_delay, trail_delay;
} Model144Config;

/* The work area: the two 33-vertex rings, the five ring origins (pad marks
 * an aimed ring) and their angles, the burst centre, 128 spark offsets, the
 * 3x3 grid and the four quads cut from it, a 14-row history of the seven
 * trail points, ten texture words and the trail ring-buffer state. */
typedef struct {
    Model144Config *G32 config;
    SVECTOR ring[66];
    SVECTOR nodes[5];
    SVECTOR angles[5];
    SVECTOR center;
    SVECTOR sparks[128];
    SVECTOR grid[9];
    SVECTOR trail[14][7];
    SVECTOR *G32 quads[4][4];
    s32 texture[10];
    s32 head;
    s32 remaining;
    u8 completed, unknown_A35[3];
    s32 elapsed;
} Model144State;

extern char gModelVariant144_D18F4[];
extern GsIMAGE gModelVariant144_D1908[];
extern Model144Config gModelVariant144_D1A20[];

s32 ModelVariant144_HandleRequest(u8 *context, s32 command);

#endif

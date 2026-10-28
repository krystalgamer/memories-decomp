#ifndef MEMORIES_DECOMP_MODEL_VARIANT170_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT170_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/libgte.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2A-byte command row, picked by command % 100: the beam, gather,
 * band and spark colours, the slot part the glow gathers at, the ring
 * radius (also the spark spread), the gather half size (also the trail
 * jitter), the outer and inner band radii, the spark half size, the beam
 * travel, gather, trail, flash and spark durations, the spacing between
 * sparks and the three start delays. */
typedef struct {
    u8 red, green, blue;
    u8 gather_red, gather_green, gather_blue;
    u8 band_red, band_green, band_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 part, unknown_0D;
    s16 radius, half_size, outer_radius, inner_radius, spark_half_size;
    s16 travel_duration, gather_duration, trail_duration, flash_duration, spark_duration;
    s16 spacing, delay, gather_delay, spark_delay;
} Model170Config;

/* The 0x10AC-byte work area: the command row, this slot's position raised
 * 1500 units, the other slot's position, the 33-point beam ring, the gathered
 * glow position, the two 17-point band rings, twenty rows of twenty trail
 * points, sixty spark positions, three texture words, the completion flag,
 * the newest trail row, the elapsed time and a frame counter. */
typedef struct {
    Model170Config *G32 config;
    SVECTOR origin;
    SVECTOR target;
    SVECTOR ring[33];
    SVECTOR gather;
    SVECTOR bands[2][17];
    SVECTOR trail[20][20];
    SVECTOR sparks[60];
    u32 texture[3];
    u8 completed;
    u8 unknown_10A1;
    s16 head;
    s32 elapsed, animation;
} Model170State;

extern GsIMAGE D_8013CFE4[];
extern Model170Config D_8013D038[];

s32 func_8013B004(u8 *context, s32 command);

#endif

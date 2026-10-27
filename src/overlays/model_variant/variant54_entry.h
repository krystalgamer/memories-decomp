#ifndef MEMORIES_DECOMP_MODEL_VARIANT54_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT54_ENTRY_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_constants.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x26-byte command row: the spiral, ring and spark colours, the ring
 * radii and height, the spiral strip width, the spark half-size and rise, the
 * flash length, the spark life, the ring count, the spark count and interval,
 * and the wait, start and ring-delay times. */
typedef struct {
    u8 red, green, blue;
    u8 ring_red, ring_green, ring_blue;
    u8 spark_red, spark_green, spark_blue;
    s16 radius, top_radius, height, width, size, rise;
    s16 flash, life, rings, sparks, interval;
    s16 wait, start, delay;
} Model54Config;

/* The work area: the ring vertex pairs, three trails of spiral points, the
 * spark offsets, the trail head, the textures, the stage and the timer. */
typedef struct {
    Model54Config *G32 config;
    SVECTOR ring[17][2];
    SVECTOR spiral[3][20];
    SVECTOR sparks[64];
    s32 head;
    s32 textures[4];
    s32 texture;
    u8 stage;
    s32 elapsed;
} Model54State;

extern GsIMAGE D_8013C648[4];
extern GsIMAGE D_8013C6B8[1];
extern Model54Config D_8013C6D4[];

s32 func_8013B004(u8 *context, s32 command);

#endif

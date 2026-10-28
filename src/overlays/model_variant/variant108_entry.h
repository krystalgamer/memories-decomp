#ifndef MEMORIES_DECOMP_MODEL_VARIANT108_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT108_ENTRY_H

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
#include "../../game/screen_projection.h"

/* One 0x28-byte command row: the line, trail, fade and spark colours, the
 * eight joints the approach lines follow, the two joints whose segment
 * leaves each trail, the fade rise and spark size, and the approach, trail,
 * fade, start delay and per-trail start times. */
typedef struct {
    u8 line_red, line_green, line_blue;
    u8 trail_red, trail_green, trail_blue;
    u8 fade_red, fade_green, fade_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 joints[8];
    u8 trail_joints[2];
    s16 unknown_16;
    s16 rise;
    s16 size;
    s16 approach;
    s16 duration;
    s16 fade;
    s16 delay;
    s16 start[2];
} Model108Config;

/* One trail: the four latest inner and outer segment end points. */
typedef struct {
    SVECTOR inner[4];
    SVECTOR outer[4];
} Model108Trail;

/* The 0xDC-byte work area: the command row, the anchor joint position at
 * the previous frame, two trails (also seen as one flat run of 16 end
 * points, which the setup fills in a single walk) and the quads they leave
 * behind, each trail's ring index, two textures and the elapsed time. */
typedef struct {
    Model108Config *G32 config;
    SVECTOR anchor;
    union {
        Model108Trail trails[2];
        SVECTOR points[16];
    } ends;
    SVECTOR quads[2][4];
    u8 index[2];
    u8 unknown_ce[2];
    u32 texture[2];
    s32 elapsed;
} Model108State;

extern GsIMAGE D_8013C52C[];
extern Model108Config D_8013C564[];

s32 func_8013B004(u8 *context, s32 command);

#endif

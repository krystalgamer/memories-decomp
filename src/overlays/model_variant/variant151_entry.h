#ifndef MEMORIES_DECOMP_MODEL_VARIANT151_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT151_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"
#include "../../psyq/sdk_internal.h"

/* One 0x14-byte command row, picked by command % 100: the ring colour, the
 * peak spark colour, the slot part the rings leave from, the ring radius, the
 * radius of the circle of targets, the spark half size, the flight duration,
 * the spacing between rings and the start delay. */
typedef struct {
    u8 red, green, blue;
    u8 spark_r, spark_g, spark_b;
    u8 part, unknown_07;
    s16 radius, spread, spark_size, duration;
    u8 spacing, unknown_11;
    s16 delay;
} Model151Config;

/* The 0x280-byte work area: the command row, a seventeen-point ring outline
 * followed by 30 ring targets, 30 ring positions, two texture words, the
 * completion flag, the elapsed time and the frame count. */
typedef struct {
    Model151Config *G32 config;
    /* The ring outline (points 0-16) and the ring targets (17-46) share one
     * array: the spark walk steps three points at a time and its last step
     * lands one point past the outline. */
    SVECTOR points[47];
    SVECTOR positions[30];
    u32 texture[2];
    u8 completed, unknown_275[3];
    s32 elapsed;
    s32 frames;
} Model151State;

extern GsIMAGE gModelVariant151_D0F90[];
extern Model151Config gModelVariant151_D0FC8[];

s32 ModelVariant151_HandleRequest(u8 *context, s32 command);

#endif

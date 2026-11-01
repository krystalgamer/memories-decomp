#ifndef MEMORIES_DECOMP_MODEL_VARIANT133_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT133_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x2C-byte command row: the fan, flash, box and spark colours, the slot
 * part the effect follows, the inner fan radius and the outer ring's extra
 * radius, the fan height, the box shell radius, two unused sizes, the spark
 * size and shell radius, the fan arc, the fan tilt, the fan open and close
 * times, the flash, box and spark durations and the start delay. */
typedef struct {
    u8 fan_red, fan_green, fan_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 box_red, box_green, box_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 part, unknown_0d;
    s16 radius, ring_radius, height;
    s16 box_radius, unknown_16, unknown_18;
    s16 spark_size, spark_radius;
    s16 arc, tilt;
    s16 open, close;
    s16 flash_duration, spark_duration;
    s16 delay;
} Model133Config;

/* The 0x2B0-byte work area: the command row, the inner and outer fan rings
 * (the outer ring also anchors the box shells and spark groups), the latched
 * part position, the box shell, four sparks per outer ring point, three
 * textures, the hit flag and the elapsed time. */
typedef struct {
    Model133Config *G32 config;
    SVECTOR inner[9];
    SVECTOR outer[9];
    SVECTOR origin;
    SVECTOR dots[32];
    SVECTOR sparks[32];
    u32 texture[3];
    u8 hit;
    u8 unknown_2a9[3];
    s32 elapsed;
} Model133State;

extern GsIMAGE gModelVariant133_D1864[];
extern Model133Config gModelVariant133_D18B8[];

s32 ModelVariant133_HandleRequest(u8 *context, s32 command);

#endif

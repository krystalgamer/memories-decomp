#ifndef MEMORIES_DECOMP_MODEL_VARIANT132_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT132_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/rand.h"
#include "../../psyq/string.h"
#include "../../game/screen_projection.h"

/* One 0x24-byte command row: the orb, spark and flash colours, the ring
 * radius, the orbs' rise per frame and height, the approach swirl radius,
 * the spark spread, the spark half size, the orb life, the approach and
 * travel times, the start delay and the time the effect stops spawning. */
typedef struct {
    u8 red, green, blue;
    u8 spark_red, spark_green, spark_blue;
    u8 unknown_06, unknown_07, unknown_08;
    u8 flash_red, flash_green, flash_blue;
    s16 unknown_0c;
    s16 radius;
    s16 rise;
    s16 height;
    s16 swirl;
    s16 spread;
    s16 half_size;
    s16 life;
    s16 approach;
    s16 travel;
    s16 delay;
    s16 end;
} Model132Config;

/* The 0x23C-byte work area: the command row, the ring outline, ten rising
 * orbs and eight sparks (each with its remaining life in pad), two textures,
 * whether the hit has been reported and the elapsed time. */
typedef struct {
    Model132Config *G32 config;
    SVECTOR ring[17];
    SVECTOR orbs[10];
    SVECTOR sparks[8];
    u8 unknown_11c[0x110];
    u32 texture[2];
    u8 started;
    s32 elapsed;
} Model132State;

extern GsIMAGE D_8013C388[];
extern Model132Config D_8013C3C0[];

s32 func_8013B004(u8 *context, s32 command);

#endif

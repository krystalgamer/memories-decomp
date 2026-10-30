#ifndef MEMORIES_DECOMP_MODEL_VARIANT806_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT806_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x38-byte command row: the trail and spark colours, the two model
 * parts the trails leave from, the glow width and height, the charge and
 * core sizes, the four core ring spins, the spark count, the spark, the
 * ember width and height, the ember count and spread, the four fade steps
 * and the six timings. */
typedef struct {
    u8 r, g, b;
    u8 spark_r, spark_g, spark_b;
    u8 part;
    u8 tail_part;
    u16 glow_width;
    u16 glow_height;
    u16 charge_size;
    u16 core_size;
    u16 spin[4];
    u16 count;
    u16 spark_size;
    u16 ember_width;
    u16 ember_height;
    u16 ember_count;
    u16 spread;
    u8 fade, unknown_25;
    u8 glow_fade, unknown_27;
    u8 flash_fade, unknown_29;
    u8 core_fade, unknown_2B;
    u16 charge_end;
    u16 first_end;
    u16 second_start;
    u16 second_end;
    u16 burst;
    u16 flash_start;
} Model806Config;

/* The work area: the core centre, the sparks with their embers, the ember
 * velocities and angles, the two trails' head and tail points, the timer,
 * the tick, the phase, the trail heads and lengths, the shown spark count,
 * the ambient level, the textures, the trail colours and glow levels, the
 * charge colour and flash level, the spark colours and the core colour. */
typedef struct {
    Model806Config *G32 config;
    SVECTOR center;
    SVECTOR pos[60];
    SVECTOR embers[60][8];
    SVECTOR velocities[60][8];
    SVECTOR angles[60][8];
    SVECTOR trail[16][2];
    SVECTOR tail[16][2];
    u32 elapsed;
    u32 tick;
    u16 phase;
    u16 head[2];
    s16 length[2];
    u16 shown;
    s32 ambient;
    s32 texture[4];
    u8 trail_r[2];
    u8 trail_g[2];
    u8 trail_b[2];
    u8 glow[2];
    union {
        struct {
            u8 r, g, b;
            u8 flash;
        } c;
        u32 word;
    } charge;
    u8 spark_r[60];
    u8 spark_g[60];
    u8 spark_b[60];
    union {
        struct {
            u8 r, g, b;
        } c;
        u32 word;
    } core;
} Model806State;

extern GsIMAGE D_8013CF68[4];
extern Model806Config D_8013CFD8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

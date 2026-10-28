#ifndef MEMORIES_DECOMP_MODEL_VARIANT148_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT148_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x1C-byte command row: the cone and spark colour, the slot part the
 * cones start from, the cone length and radius, the target spread, the
 * spark half size and shell radius, the per-cone stagger, the flight, burst
 * and flash durations, two angle rates and the start delay. */
typedef struct {
    u8 red, green, blue, part;
    s16 length;          /* 0x04 */
    s16 radius;          /* 0x06 */
    s16 spread;          /* 0x08 */
    s16 half_size;       /* 0x0A */
    s16 shell;           /* 0x0C */
    s16 stagger;         /* 0x0E */
    s16 fly_duration;    /* 0x10 */
    s16 burst_duration;  /* 0x12 */
    s16 flash_duration;  /* 0x14 */
    s16 spin;            /* 0x16 */
    s16 twist;           /* 0x18 */
    s16 delay;           /* 0x1A */
} Model148Config;

/* The 0x384-byte work area: the command row, a 30-point cone mesh, the head
 * and target of each of 24 cones, 32 spark offsets, the texture, the hit and
 * flash flags and the elapsed time. */
typedef struct {
    Model148Config *G32 config;
    SVECTOR cone[30];        /* 0x004 */
    SVECTOR heads[24];       /* 0x0F4 */
    SVECTOR targets[24];     /* 0x1B4 */
    SVECTOR sparks[32];      /* 0x274 */
    u32 texture[2];          /* 0x374 */
    u8 hit;                  /* 0x37C */
    u8 flash;                /* 0x37D */
    s32 elapsed;             /* 0x380 */
} Model148State;

extern GsIMAGE D_8013C728[];
extern Model148Config D_8013C744[];

s32 func_8013B004(u8 *context, s32 command);

#endif

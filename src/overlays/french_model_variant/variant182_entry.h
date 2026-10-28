#ifndef MEMORIES_DECOMP_MODEL_VARIANT182_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT182_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x24-byte command row: the sprite colour, an unused second colour,
 * the slot part the sprites start from, the sprite count and half size, the
 * scatter radius, the burst ring's inner and outer radii, the fly, hold,
 * pulse, converge and burst durations, the per-sprite stagger and the
 * start times of the flight, the pulse and the convergence. */
typedef struct {
    u8 red, green, blue;
    u8 unknown_3[3];
    u8 part;                 /* 0x06 */
    u8 count;                /* 0x07 */
    s16 half_size;           /* 0x08 */
    s16 spread;              /* 0x0A */
    s16 inner_radius;        /* 0x0C */
    s16 outer_radius;        /* 0x0E */
    s16 fly_duration;        /* 0x10 */
    s16 hold_duration;       /* 0x12 */
    s16 pulse_period;        /* 0x14 */
    s16 converge_duration;   /* 0x16 */
    s16 burst_duration;      /* 0x18 */
    s16 stagger;             /* 0x1A */
    s16 unknown_1C;
    s16 delay;               /* 0x1E */
    s16 pulse_delay;         /* 0x20 */
    s16 converge_start;      /* 0x22 */
} Model182Config;

/* The 0x63C-byte work area: the command row, up to 64 sprite positions and
 * their scatter offsets, the convergence target, a 2x33-point burst ring,
 * five texture words, the hit flag, the elapsed time and a frame counter. */
typedef struct {
    Model182Config *G32 config;
    SVECTOR sprites[64];     /* 0x004 */
    SVECTOR offsets[64];     /* 0x204 */
    SVECTOR target;          /* 0x404 */
    SVECTOR ring[66];        /* 0x40C */
    u32 texture[5];          /* 0x61C */
    u8 hit;                  /* 0x630 */
    s32 elapsed;             /* 0x634 */
    s32 frame;               /* 0x638 */
} Model182State;

extern GsIMAGE D_8013CCD8[];
extern Model182Config D_8013CD64[];

s32 func_8013B004(u8 *context, s32 command);

#endif

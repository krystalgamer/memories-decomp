#ifndef MEMORIES_DECOMP_MODEL_VARIANT173_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT173_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x1A-byte command row, picked by command % 100: the cube colour, the
 * beam's outer and inner ring colours, the cube half size, the inner and outer
 * ring radii, the drift and beam durations, the spacing between particles,
 * the particle count and the start time. */
typedef struct {
    u8 red, green, blue, end_red, end_green, end_blue, start_red, start_green, start_blue, unknown_9;
    s16 size, inner, outer, duration, fade, spacing, count, delay;
} Model173Config;

/* The 0x6D4-byte work area: the command row, 128 particle positions, the
 * eight cube corners, a 34-point double ring, the six faces' corner pointers,
 * a four-bit cube mask per particle, five texture words, the completion flag,
 * the elapsed time and a frame count. */
typedef struct {
    Model173Config *G32 config;
    SVECTOR positions[128];
    SVECTOR corners[8];
    SVECTOR ring[34];
    SVECTOR *G32 faces[24];
    u16 masks[128];
    u32 texture[5];
    u8 completed, unknown_6C9[3];
    s32 elapsed;
    s32 frames;
} Model173State;

extern GsIMAGE D_8013C2F0[];
extern Model173Config D_8013C37C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

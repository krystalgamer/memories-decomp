#ifndef MEMORIES_DECOMP_MODEL_VARIANT175_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT175_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x20-byte command row: the ring colour, three unused colour bytes, the
 * slot part the rings start from, the ring radius and thickness, the near
 * ring's depth, two unused words, the flight duration, the spark lifetime,
 * the spacing between rings, an unused word, the ring count and the start
 * delays of the first and second waves. */
typedef struct {
    u8 red, green, blue, unknown_03[3], part, unknown_07;
    s16 radius, thickness, near_z, unknown_0E, unknown_10, duration, lifetime;
    s16 interval, unknown_18, count, delay, second_delay;
} Model175Config;

/* The 0x3AC-byte work area: the command row, the three 17-point rings, the
 * ring positions of both waves, the completion flag and the elapsed time. */
typedef struct {
    Model175Config *G32 config;
    SVECTOR ring[51];
    SVECTOR positions[65];
    u8 completed, unknown_3a5[3];
    s32 elapsed;
} Model175State;

extern Model175Config gModelVariant175_D1AEC[];

s32 ModelVariant175_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT111_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT111_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x1C-byte command row: the particles' colour and source part, the quad
 * half size, the depth they gather at, the extra depth of the last leg, and
 * the leg duration, spacing, count and start delay. */
typedef struct {
    u8 red, green, blue, unknown_03[3], part, unknown_07;
    s16 half_size, unknown_0A, distance, spread, unknown_10;
    s16 travel_duration, spacing, unknown_16, count, delay;
} Model111Config;

typedef struct {
    Model111Config *G32 config;
    SVECTOR positions[64];
    SVECTOR velocities[64];
    s32 texture[1];
    s32 elapsed;
    u8 completed, unknown_40D[3];
} Model111State;

extern const VECTOR D_8013BCF8;
extern GsIMAGE D_8013BD08[];
extern Model111Config D_8013BD24[];
s32 func_8013B004(u8 *context, s32 command);

#endif

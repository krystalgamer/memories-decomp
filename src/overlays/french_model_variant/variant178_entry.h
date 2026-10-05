#ifndef FRENCH_MODEL_VARIANT178_ENTRY_H
#define FRENCH_MODEL_VARIANT178_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 r, g, b, burst_r, burst_g, burst_b, part, field_07;
    s16 half_size, spread, burst_half_size, rise, velocity_spread;
    s16 count, spacing, travel, fade, burst_duration, delay;
} Model178Config;

typedef struct {
    Model178Config *G32 config;
    SVECTOR positions[64];
    SVECTOR endpoints[64];
    SVECTOR velocities[4];
    u32 texture[2];
    u8 field_42C[0x1C];
    u8 completed;
    u8 field_449[3];
    s32 elapsed;
} Model178State;

extern const VECTOR D_8013BD3C;
extern GsIMAGE D_8013BD4C[];
extern Model178Config D_8013BD84[];
s32 func_8013B004(u8 *context, s32 command);

#endif

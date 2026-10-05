#ifndef FRENCH_MODEL_VARIANT153_ENTRY_H
#define FRENCH_MODEL_VARIANT153_ENTRY_H

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
    u8 red, green, blue;
    u8 burst_red, burst_green, burst_blue, part, unknown_07;
    s16 half_size, spread, burst_half_size, rise_height, burst_spread;
    s16 count, spacing, travel_duration, fade_duration, burst_duration, delay;
} Model153Config;

typedef struct {
    Model153Config *G32 config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    SVECTOR burst_offsets[4];
    u32 texture[9];
    u8 completed;
    u8 unknown_449[3];
    s32 elapsed;
} Model153State;

extern const VECTOR D_8013BE0C;
extern GsIMAGE D_8013BE1C[];
extern Model153Config D_8013BF18[];
s32 func_8013B004(u8 *context, s32 command);

#endif

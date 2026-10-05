#ifndef FRENCH_MODEL_VARIANT129_ENTRY_H
#define FRENCH_MODEL_VARIANT129_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

typedef struct {
    u8 red, green, blue, flash_red, flash_green, flash_blue, part, unknown_07;
    s16 half_size, unknown_0A[5];
    s16 travel_duration, flash_duration, spacing, unknown_1A, count, delay;
} Model129Config;

typedef struct {
    Model129Config *G32 config;
    SVECTOR positions[64];
    SVECTOR velocities[64];
    s32 texture[2];
    s32 elapsed;
    u8 completed, effect_started, unknown_412[2];
} Model129State;

extern const VECTOR D_8013BFE8;
extern GsIMAGE D_8013BFF8[];
extern Model129Config D_8013C030[];
s32 func_8013B004(u8 *context, s32 command);

#endif

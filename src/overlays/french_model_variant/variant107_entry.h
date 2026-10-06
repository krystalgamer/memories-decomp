#ifndef FRENCH_MODEL_VARIANT107_ENTRY_H
#define FRENCH_MODEL_VARIANT107_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_effect_state.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 red, green, blue, line_red, line_green, line_blue, part, unknown_07;
    s16 radius, thickness, near_z, extra_z, minimum_scale, duration, stagger;
    s16 unknown_16, count, delay;
} Model107Config;

typedef struct {
    Model107Config *G32 config;
    SVECTOR ring[51];
    SVECTOR positions[25];
    u8 unknown_264[0x140];
    u8 completed, unknown_3a5[3];
    s32 elapsed;
    u8 started, unknown_3ad[3];
} Model107State;

extern Model107Config D_8013C198[];
s32 func_8013B004(u8 *context, s32 command);

#endif

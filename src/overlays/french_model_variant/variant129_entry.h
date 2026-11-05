#ifndef FRENCH_MODEL_VARIANT129_ENTRY_H
#define FRENCH_MODEL_VARIANT129_ENTRY_H

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

typedef struct {
    u8 red, green, blue, flash_red, flash_green, flash_blue, part, unknown_07;
    s16 half_size, unknown_0A[5];
    s16 travel_duration, flash_duration, spacing, unknown_1A, count, delay;
} Model129Config;

typedef struct {
    Model129Config *config;
    SVECTOR positions[64];
    SVECTOR velocities[64];
    s32 texture[2];
    s32 elapsed;
    u8 completed, effect_started, unknown_412[2];
} Model129State;

extern const VECTOR gFrenchModelVariant129_UnitScale;
extern GsIMAGE gFrenchModelVariant129_aImages[];
extern Model129Config gFrenchModelVariant129_aConfigs[];
s32 FrenchModelVariant129_HandleRequest(u8 *context, s32 command);

#endif

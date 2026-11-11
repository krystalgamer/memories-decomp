#ifndef FRENCH_MODEL_VARIANT141_ENTRY_H
#define FRENCH_MODEL_VARIANT141_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, sprite_red, sprite_green, sprite_blue;
    u8 point_gray, unknown_07[2], part;
    s16 grid_half_size, travel_depth, sprite_half_size, offset_spread, point_spread;
    s16 travel_duration, sprite_duration, point_duration, spacing, unknown_1c;
    s16 count, travel_delay, burst_delay;
} Model141Config;

typedef struct {
    Model141Config *config;
    SVECTOR positions[28];
    u8 unknown_e4[0x120];
    SVECTOR offsets[32], velocities[32];
    s32 texture[2], elapsed;
    u8 completed, unknown_411[3];
} Model141State;

extern GsIMAGE gFrenchModelVariant141_aImages[];
extern Model141Config gFrenchModelVariant141_aConfigs[];
s32 FrenchModelVariant141_HandleRequest(u8 *context, s32 command);

#endif

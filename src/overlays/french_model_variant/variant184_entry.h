#ifndef FRENCH_MODEL_VARIANT184_ENTRY_H
#define FRENCH_MODEL_VARIANT184_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, unknown_03[3], part, count;
    s16 moving_half_size, spread, inner_radius, outer_radius, lift;
    s16 burst_half_size, travel_duration, burst_duration, interval, delay;
} Model184Config;

typedef struct {
    Model184Config *config;
    SVECTOR positions[80];
    u8 unknown_284[0x80];
    SVECTOR targets[80];
    u8 unknown_584[0x80];
    SVECTOR origin;
    SVECTOR rings[34];
    u32 texture[3];
    u8 completed, unknown_729[3];
    s32 elapsed, updates;
    u8 command_group, unknown_735[3];
} Model184State;

extern GsIMAGE gFrenchModelVariant184_aImages[];
extern Model184Config gFrenchModelVariant184_aConfigs[];
s32 FrenchModelVariant184_HandleRequest(u8 *context, s32 command);

#endif

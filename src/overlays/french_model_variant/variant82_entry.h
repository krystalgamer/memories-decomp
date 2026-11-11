#ifndef FRENCH_MODEL_VARIANT82_ENTRY_H
#define FRENCH_MODEL_VARIANT82_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, unknown_03;
    s16 part, spread, target_spread, half_size, y_bias, travel_duration;
    s16 count, spark_divisor, delay, rate, unknown_18, end_time;
} Model82Config;

typedef struct {
    Model82Config *config;
    SVECTOR positions[201];
    u8 unknown_64c[0x9b8];
    SVECTOR velocities[201];
    u8 unknown_164c[0x9b8];
    u8 remaining[200], unknown_20cc[0x138];
    s32 texture, elapsed;
    u8 completed, unknown_220d[3];
} Model82State;

extern GsIMAGE gFrenchModelVariant82_aImages[];
extern Model82Config gFrenchModelVariant82_aConfigs[];
s32 FrenchModelVariant82_HandleRequest(u8 *context, s32 command);

#endif

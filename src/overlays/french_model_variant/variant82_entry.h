#ifndef FRENCH_MODEL_VARIANT82_ENTRY_H
#define FRENCH_MODEL_VARIANT82_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
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
    Model82Config *G32 config;
    SVECTOR positions[201];
    u8 unknown_64c[0x9b8];
    SVECTOR velocities[201];
    u8 unknown_164c[0x9b8];
    u8 remaining[200], unknown_20cc[0x138];
    s32 texture, elapsed;
    u8 completed, unknown_220d[3];
} Model82State;

extern GsIMAGE D_8013BDF4[];
extern Model82Config D_8013BE10[];
s32 func_8013B004(u8 *context, s32 command);

#endif

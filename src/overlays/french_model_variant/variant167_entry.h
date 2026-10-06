#ifndef FRENCH_MODEL_VARIANT167_ENTRY_H
#define FRENCH_MODEL_VARIANT167_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, particle_red, particle_green, particle_blue;
    u8 unknown_06[2];
    s16 amplitude, half_size, radius, fade_duration, unknown_10;
    s16 particle_duration, particle_delay, stagger, count, delay;
} Model167Config;

typedef struct {
    Model167Config *G32 config;
    SVECTOR points[64];
    s16 offsets[256], increments[256];
    s16 phase, scroll;
    u32 texture[2];
    u8 started;
    s32 elapsed;
} Model167State;

extern GsIMAGE D_8013BEE8[];
extern Model167Config D_8013BF20[];
s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT144_ENTRY_H
#define FRENCH_MODEL_VARIANT144_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/func_80058938.h"
#include "../../game/func_80058E1C.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, particle_red, particle_green, particle_blue;
    u8 glow_red, glow_green, glow_blue, unknown_09;
    s16 parts[8];
    s16 radius, half_size, gradient_duration, particle_duration, glow_fade_in, glow_duration;
    s16 tint_starts[8], tint_durations[8], tint_ramp_durations[8];
    s16 main_delay, glow_delay;
} Model144Config;

typedef struct {
    Model144Config *G32 config;
    u8 unknown_004[24];
    SVECTOR points[64];
    s32 texture[1], elapsed;
    u8 started, frame_step_override;
} Model144State;

extern GsIMAGE D_8013BEC0[];
extern Model144Config D_8013BEDC[];
s32 func_8013B004(u8 *context, s32 command);

#endif

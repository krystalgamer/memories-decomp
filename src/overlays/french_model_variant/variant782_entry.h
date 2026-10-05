#ifndef FRENCH_MODEL_VARIANT782_ENTRY_H
#define FRENCH_MODEL_VARIANT782_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    s16 half_width, unknown_02, height, unknown_06;
    s16 x_offset, side_end_z, side_height, unknown_0E;
    s32 growth_rate;
    s16 debris_count, debris_half_size, debris_spread, debris_speed;
    s16 dot_count, dot_speed, initial_delay, burst_end;
    s32 duration;
} Model782Config;

typedef struct {
    Model782Config *G32 config;
    SVECTOR origin;
    SVECTOR debris_positions[16];
    u8 unknown_08C[0x180];
    SVECTOR debris_velocities[16];
    u8 unknown_28C[0x180];
    SVECTOR dot_positions[64];
    SVECTOR dot_velocities[64];
    DVECTOR screen[64];
    s32 texture[5];
    s32 level;
    u16 depths[64];
    s16 animation, elapsed, phase, growth_age;
    u8 shade;
    u8 unknown_9AD[3];
} Model782State;

extern GsIMAGE D_8013C4C0[];
extern Model782Config D_8013C54C[];
s32 func_8013B004(u8 *context, s32 command);

#endif

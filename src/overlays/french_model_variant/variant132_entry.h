#ifndef FRENCH_MODEL_VARIANT132_ENTRY_H
#define FRENCH_MODEL_VARIANT132_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 red, green, blue, flash_red, flash_green, flash_blue;
    u8 ring_red, ring_green, ring_blue, unknown_09;
    s16 radius, curve_width, path_height, half_size, ring_height;
    s16 radial_count, ring_count, particle_count;
    s16 particle_duration, flash_duration, ring_duration, delay;
} Model132Config;

typedef struct {
    Model132Config *G32 config;
    SVECTOR path[128];
    SVECTOR ring[34];
    u16 position[4];
    s32 texture[1], elapsed;
    u8 started;
} Model132State;

extern GsIMAGE D_8013BEC8[];
extern Model132Config D_8013BEE4[];
s32 func_8013B004(u8 *context, s32 command);

#endif

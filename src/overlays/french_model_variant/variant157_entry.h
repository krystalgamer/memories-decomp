#ifndef FRENCH_MODEL_VARIANT157_ENTRY_H
#define FRENCH_MODEL_VARIANT157_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, sprite_red, sprite_green, sprite_blue;
    u8 trail_red, trail_green, trail_blue, unknown_09;
    s16 radius, half_size, fall_distance, ring_duration, fall_duration;
    s16 unknown_14, flash_duration, spacing, delay;
} Model157Config;

typedef struct {
    Model157Config *G32 config;
    SVECTOR origin, ring[33], starts[32], trails[192];
    u32 texture[1];
    u8 completed, toggle, unknown_81a[2];
    s32 elapsed;
} Model157State;

extern GsIMAGE D_8013C068[];
extern Model157Config D_8013C084[];
s32 func_8013B004(u8 *context, s32 command);

#endif

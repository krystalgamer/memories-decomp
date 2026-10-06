#ifndef FRENCH_MODEL_VARIANT143_ENTRY_H
#define FRENCH_MODEL_VARIANT143_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, sprite_red, sprite_green, sprite_blue, part, count;
    s16 length, half_width, spread, height_spread, sprite_half_size;
    s16 travel_duration, hold_duration, sprite_duration;
    s16 rotation_x, rotation_y, rotation_z, spacing, delay;
} Model143Config;

typedef struct {
    SVECTOR *G32 a, *G32 b, *G32 c;
} Model143Face;

typedef struct {
    Model143Config *G32 config;
    SVECTOR positions[55];
    u8 unknown_1bc[0x48];
    SVECTOR vertices[5], targets[55];
    u8 unknown_3e4[0x48];
    SVECTOR rotations[55];
    u8 unknown_5e4[0x48];
    Model143Face faces[6];
    CVECTOR colors[6];
    u32 texture[1];
    s32 elapsed;
    u8 completed, unknown_695[3];
} Model143State;

extern GsIMAGE D_8013C004[];
extern Model143Config D_8013C020[];
s32 func_8013B004(u8 *context, s32 command);

#endif

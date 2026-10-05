#ifndef FRENCH_MODEL_VARIANT177_ENTRY_H
#define FRENCH_MODEL_VARIANT177_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/screen_projection.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

typedef struct {
    u8 red, green, blue;
    u8 grid_red, grid_green, grid_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 part, count, unknown_0b;
    s16 half_width, height, depth, grid_half_size;
    s16 travel_duration, fade_duration, flash_duration, spacing, delay;
} Model177Config;

typedef struct {
    Model177Config *G32 config;
    SVECTOR ribbon[34];
    SVECTOR positions[64];
    SVECTOR origin;
    u8 unknown_31C[0x1F8];
    SVECTOR vertices[9];
    SVECTOR *G32 faces[16];
    u32 texture[2];
    u8 completed;
    u8 unknown_5a5[3];
    s32 elapsed, animation;
} Model177State;

extern const VECTOR D_8013C19C;
extern GsIMAGE D_8013C1AC[];
extern Model177Config D_8013C200[];
s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT117_ENTRY_H
#define FRENCH_MODEL_VARIANT117_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, part;
    s16 size, arc_height, spread, radius, flash_size;
    s16 count, rows, travel_duration, fade_duration, spacing, delay;
    u8 unknown_1a, tile_size, frames, unknown_1d;
} Model117Config;

typedef struct {
    Model117Config *G32 config;
    SVECTOR positions[40];
    u8 unknown_144[0xc0];
    SVECTOR targets[40];
    u8 unknown_344[0xc0];
    SVECTOR ends[40];
    u8 unknown_544[0xc0];
    u8 rows[40], unknown_62c[0x18];
    u8 frames[40], unknown_66c[0x18];
    s16 scales[40];
    u8 unknown_6d4[0x30];
    s32 textures[6];
    u8 completed, unknown_71d[3];
    s32 elapsed;
} Model117State;

extern GsIMAGE D_8013C09C[];
extern Model117Config D_8013C144[];
s32 func_8013B004(u8 *context, s32 command);

#endif

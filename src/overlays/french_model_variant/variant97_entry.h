#ifndef FRENCH_MODEL_VARIANT97_ENTRY_H
#define FRENCH_MODEL_VARIANT97_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, particle_red, particle_green, particle_blue, part, unknown_07;
    s16 height, radius, half_size, spread, lift;
    s16 path_count, ring_count, particle_count;
    s16 beam_duration, ring_duration, particle_duration, stagger, spin, delay;
} Model97Config;

typedef struct {
    Model97Config *G32 config;
    SVECTOR path[32];
    u8 unknown_104[0x100];
    SVECTOR ring[16];
    SVECTOR offsets[8];
    u8 unknown_2c4[0x40];
    SVECTOR target;
    u8 initialized, unknown_30d[3];
    s32 texture[3];
    u8 completed, unknown_31d[3];
    s32 elapsed;
} Model97State;

extern GsIMAGE D_8013C168[];
extern Model97Config D_8013C1BC[];
s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT163_ENTRY_H
#define FRENCH_MODEL_VARIANT163_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 red, green, blue, ring_red, ring_green, ring_blue, first_part, second_part;
    s16 radius, thickness, stagger, particle_duration, split_time;
    s16 flash_duration, ring_duration, delay;
} Model163Config;

typedef struct {
    Model163Config *G32 config;
    SVECTOR points[130];
    SVECTOR *G32 quads[16];
    u32 texture[2];
    u8 completed, unknown_45d[3];
    s32 elapsed;
} Model163State;

extern GsIMAGE D_8013BEB8[];
extern Model163Config D_8013BEF0[];
s32 func_8013B004(u8 *context, s32 command);

#endif

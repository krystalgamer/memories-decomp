#ifndef FRENCH_MODEL_VARIANT173_ENTRY_H
#define FRENCH_MODEL_VARIANT173_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, part;
    s16 half_size, spread, unknown_08;
    s16 gather_duration, travel_duration, burst_duration;
    s16 group_size, count, spacing, delay, unknown_18;
} Model173Config;

typedef struct {
    Model173Config *G32 config;
    SVECTOR positions[256];
    SVECTOR offsets[256];
    SVECTOR center;
    u32 texture[10];
    u8 completed;
    u8 unknown_1035[0x203];
    s32 elapsed;
    u8 mode, unknown_123D[3];
    s32 frames;
} Model173State;

extern const VECTOR D_8013BEB0;
extern GsIMAGE D_8013BEC0[];
extern Model173Config D_8013BFD8[];
s32 func_8013B004(u8 *context, s32 command);

#endif

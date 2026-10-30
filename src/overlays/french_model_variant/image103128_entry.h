#ifndef FRENCH_MODEL_IMAGE103128_ENTRY_H
#define FRENCH_MODEL_IMAGE103128_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 parts[4];
    u16 start;
    u16 end;
    u16 unknown_08;
} Image103128Config;

typedef union {
    struct {
        u16 phase;
        u16 states[4][8];
        u16 count;
    } fields;
    /* The footer reads phase and the first projectile state as one word. */
    u32 word;
} Image103128Control;

typedef struct {
    Image103128Config *G32 config;
    SVECTOR positions[4][8];
    SVECTOR velocities[4][8];
    SVECTOR impacts[4][8];
    u32 elapsed;
    u32 tick;
    u32 sizes[4][8];
    Image103128Control control;
    s32 ambient;
    u16 tpage;
    u16 clut;
    s32 textures[2];
    u8 alpha[4][8];
    CVECTOR colors[4][8];
} Image103128State;

extern VECTOR D_8013BEEC;
extern GsIMAGE D_8013BEFC[2];
extern Image103128Config D_8013BF34[];

#endif

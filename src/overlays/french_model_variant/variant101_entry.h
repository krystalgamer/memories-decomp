#ifndef FRENCH_MODEL_VARIANT101_ENTRY_H
#define FRENCH_MODEL_VARIANT101_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 particle_r, particle_g, particle_b;
    u8 ring_r, ring_g, ring_b;
    s16 speed;
    s16 radius;
    s16 count;
    s16 delay;
    s16 duration;
} Model101Config;

typedef struct {
    Model101Config *G32 config;
    SVECTOR positions[96];
    SVECTOR velocities[96];
    SVECTOR ring[66];
    DVECTOR projected[96];
    u16 depths[96];
    s32 texture;
    u8 completed;
    u8 field_a59[3];
    s32 elapsed;
} Model101State;

extern const VECTOR D_8013BAE4;
extern u8 D_8013BAF4[];
extern u8 D_8013BB10[];
s32 func_8013B004(u8 *context, s32 command);

#endif

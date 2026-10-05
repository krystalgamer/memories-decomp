#ifndef FRENCH_MODEL_VARIANT77_ENTRY_H
#define FRENCH_MODEL_VARIANT77_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 line_r, line_g, line_b;
    u8 particle_r, particle_g, particle_b;
    u8 part, field_07;
    s16 travel, duration, width, amplitude, size, speed, delay;
} Model77Config;

typedef struct {
    Model77Config *G32 config;
    SVECTOR position;
    SVECTOR velocities[32];
    u32 texture[1];
    u8 completed;
    u8 field_111[3];
    s32 elapsed;
} Model77State;

extern const VECTOR D_8013BC18;
extern GsIMAGE D_8013BC28[];
extern Model77Config D_8013BC44[];
s32 func_8013B004(u8 *context, s32 command);

#endif

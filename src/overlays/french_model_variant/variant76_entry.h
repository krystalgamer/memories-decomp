#ifndef FRENCH_MODEL_VARIANT76_ENTRY_H
#define FRENCH_MODEL_VARIANT76_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/gpu_packets.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 color[3];
    u8 count;
    u8 lifetime;
    u8 fade;
    u8 interval;
    u8 part;
    u8 field_08[8];
    s16 size;
    s16 angle_y_range;
    s16 angle_x_range;
    s16 delay;
} Model76Config;

typedef struct {
    SVECTOR *G32 a;
    SVECTOR *G32 b;
    SVECTOR *G32 c;
} Model76Face;

typedef struct {
    Model76Config *G32 config;
    SVECTOR positions[256];
    SVECTOR velocities[256];
    SVECTOR rotations[256];
    SVECTOR vertices[4];
    s16 life[256];
    s32 elapsed;
    u8 completed;
    u8 field_1a29[3];
    Model76Face faces[4];
} Model76State;

extern Model76Config D_8013BB98[];
s32 func_8013B004(u8 *context, s32 command);

#endif

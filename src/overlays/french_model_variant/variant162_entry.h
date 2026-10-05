#ifndef FRENCH_MODEL_VARIANT162_ENTRY_H
#define FRENCH_MODEL_VARIANT162_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80058E1C.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

typedef struct {
    u8 r, g, b, quad_r, quad_g, quad_b, part, field_07;
    s16 radius, half_width, line_duration, quad_duration, line_delay, quad_delay;
} Model162Config;

typedef struct {
    Model162Config *G32 config;
    SVECTOR points[256];
    SVECTOR anchor, rotation;
    s32 texture[1];
    u8 completed;
    u8 field_819[3];
    s32 elapsed;
} Model162State;

extern const VECTOR D_8013BDAC;
extern GsIMAGE D_8013BDBC[];
extern Model162Config D_8013BDD8[];
s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT162_ENTRY_H
#define FRENCH_MODEL_VARIANT162_ENTRY_H

#include "../../types.h"
#include "../../game/model.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

typedef struct {
    u8 r, g, b, quad_r, quad_g, quad_b, part, field_07;
    s16 radius, half_width, line_duration, quad_duration, line_delay, quad_delay;
} Model162Config;

typedef struct {
    Model162Config *config;
    SVECTOR points[256];
    SVECTOR anchor, rotation;
    s32 texture[1];
    u8 completed;
    u8 field_819[3];
    s32 elapsed;
} Model162State;

extern const VECTOR gFrenchModelVariant162_UnitScale;
extern GsIMAGE gFrenchModelVariant162_aImages[];
extern Model162Config gFrenchModelVariant162_aConfigs[];
s32 FrenchModelVariant162_HandleRequest(u8 *context, s32 command);

#endif

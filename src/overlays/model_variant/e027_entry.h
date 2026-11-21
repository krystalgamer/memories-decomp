#ifndef FRENCH_MODEL_VARIANT96_ENTRY_H
#define FRENCH_MODEL_VARIANT96_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 r, g, b, part;
    s16 outer_radius, inner_radius, segments, count;
    s16 duration, flash_duration, spacing, delay;
} FrenchModel96Config;

typedef struct {
    FrenchModel96Config *config;
    u8 field_04[8];
    SVECTOR outer[65];
    SVECTOR inner[65];
    SVECTOR positions[128];
    s32 scales[128];
    SVECTOR rotation;
    u8 prepared;
    u8 field_A25[3];
    u32 texture[1];
    u8 completed;
    u8 field_A2D[3];
    s32 elapsed;
} FrenchModel96State;

extern const VECTOR gFrenchModelVariant96_UnitScale;
extern GsIMAGE gFrenchModelVariant96_aImages[];
extern FrenchModel96Config gFrenchModelVariant96_aConfigs[];
s32 FrenchModelVariant96_HandleRequest(FrenchModel96State *context, s32 command);

#endif

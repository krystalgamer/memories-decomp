#ifndef FRENCH_MODEL_VARIANT198_ENTRY_H
#define FRENCH_MODEL_VARIANT198_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    u8 ray_r, ray_g, ray_b, ring_r, ring_g, ring_b;
    u8 field_06[2];
    s16 ray_height, ray_half_width, spread, ring_radius, ring_width, ring_y;
    s16 ray_duration, ray_delay, ring_duration, flash_duration;
    s16 ray_spacing, phase_spacing, phase_count, start_delay;
    s16 field_24, field_26;
} Model198Config;

typedef struct {
    Model198Config *config;
    SVECTOR rays[32];
    SVECTOR opponent;
    SVECTOR ring[66];
    SVECTOR origin;
    s32 texture[2];
    s32 elapsed;
    u8 completed, triggered;
    u8 field_332[2];
} Model198State;

extern const VECTOR gFrenchModelVariant198_UnitScale;
extern GsIMAGE gFrenchModelVariant198_aImages[];
extern Model198Config gFrenchModelVariant198_aConfigs[];
s32 FrenchModelVariant198_HandleRequest(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT199_ENTRY_H
#define FRENCH_MODEL_VARIANT199_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 ray_r, ray_g, ray_b, grid_r, grid_g, grid_b;
    u8 sphere_r, sphere_g, sphere_b, field_09;
    s16 ray_size, ray_radius, ray_height, grid_radius;
    s16 sphere_size, sphere_radius, sphere_fall, ray_fade_in;
    s16 flash_duration, grid_duration, ray_duration, sphere_duration;
    s16 ray_spacing, ray_group_size, ray_delay, burst_delay;
} Model199Config;

typedef struct {
    Model199Config *config;
    SVECTOR rays[128];
    SVECTOR anchor;
    SVECTOR grid[9];
    SVECTOR sphere[32];
    SVECTOR *quads[16];
    s32 texture[5];
    s32 elapsed;
    u8 completed;
    u8 field_5AD[3];
} Model199State;

extern const VECTOR gFrenchModelVariant199_D1594;
extern GsIMAGE gFrenchModelVariant199_D15A4[];
extern Model199Config gFrenchModelVariant199_aConfigs[];
s32 FrenchModelVariant199_HandleRequest(u8 *context, s32 command);

#endif

#ifndef FRENCH_MODEL_VARIANT77_ENTRY_H
#define FRENCH_MODEL_VARIANT77_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
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
    Model77Config *config;
    SVECTOR position;
    SVECTOR velocities[32];
    u32 texture[1];
    u8 completed;
    u8 field_111[3];
    s32 elapsed;
} Model77State;

extern const VECTOR gFrenchModelVariant77_UnitScale;
extern GsIMAGE gFrenchModelVariant77_aImages[];
extern Model77Config gFrenchModelVariant77_aConfigs[];
s32 FrenchModelVariant77_HandleRequest(u8 *context, s32 command);

#endif

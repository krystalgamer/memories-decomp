#ifndef FRENCH_MODEL_IMAGE119708_ENTRY_H
#define FRENCH_MODEL_IMAGE119708_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model.h"
#include "../../game/model_texture_upload.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    ModelVariantGlideConfig *config;
    VECTOR offset, velocity;
    SVECTOR particles[128];
    u32 scale, elapsed;
    u16 phase, live_count;
    u16 speeds[128], ages[128];
    s32 ambient, textures[1];
} Image119708State;

extern VECTOR gFrenchModelVariant819_UnitScale;
extern GsIMAGE gFrenchModelVariant819_aImages[1];
extern ModelVariantGlideConfig gFrenchModelVariant819_aConfigs[];

#endif

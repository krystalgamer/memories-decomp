#ifndef FRENCH_MODEL_VARIANT131_ENTRY_H
#define FRENCH_MODEL_VARIANT131_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/screen_projection.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    ModelVariantBeamParticleConfig *config;
    SVECTOR ring[14];
    SVECTOR origin;
    SVECTOR particles[32];
    s32 texture[2];
    s32 elapsed;
    u8 command_group, completed, unknown_18A[2];
} Model131State;

extern GsIMAGE gFrenchModelVariant131_aImages[];
extern ModelVariantBeamParticleConfig gFrenchModelVariant131_aConfigs[];
s32 FrenchModelVariant131_HandleRequest(u8 *context, s32 command);

#endif

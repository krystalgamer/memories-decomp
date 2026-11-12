#ifndef FRENCH_MODEL_VARIANT131_ENTRY_H
#define FRENCH_MODEL_VARIANT131_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

extern GsIMAGE gFrenchModelVariant131_aImages[];
extern ModelVariantBeamParticleConfig gFrenchModelVariant131_aConfigs[];
s32 FrenchModelVariant131_HandleRequest(u8 *context, s32 command);

#endif

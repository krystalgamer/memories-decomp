#ifndef MEMORIES_DECOMP_MODEL_VARIANT158_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT158_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

extern GsIMAGE gModelVariant158_aImages[];
extern ModelVariantTravelBurstConfig gModelVariant158_aConfigs[];

s32 ModelVariant158_HandleRequest(ModelVariantTravelBurstState *context, s32 command);

#endif

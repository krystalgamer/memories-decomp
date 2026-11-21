#ifndef MEMORIES_DECOMP_MODEL_VARIANT182_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT182_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

extern GsIMAGE gFrenchModelVariant182_aImages[];
extern ModelVariantGatherConfig gFrenchModelVariant182_aConfigs[];

s32 FrenchModelVariant182_HandleRequest(ModelVariantGatherState *context, s32 command);

#endif

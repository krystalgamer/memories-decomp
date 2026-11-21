#ifndef MEMORIES_DECOMP_MODEL_VARIANT180_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT180_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_slot_support.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

extern GsIMAGE gModelVariant180_aImages[];
extern ModelVariantBeamRingConfig gModelVariant180_aConfigs[];

s32 ModelVariant180_HandleRequest(ModelVariantBeamRingState *context, s32 command);

#endif

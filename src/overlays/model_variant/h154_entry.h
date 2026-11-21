#ifndef MEMORIES_DECOMP_MODEL_VARIANT154_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT154_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"

extern GsIMAGE gModelVariant154_aImages[];
extern ModelVariantGridBeamConfig gModelVariant154_aConfigs[];

s32 ModelVariant154_HandleRequest(ModelVariantGridBeamState *context, s32 command);

#endif

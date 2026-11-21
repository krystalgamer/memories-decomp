#ifndef MEMORIES_DECOMP_MODEL_VARIANT801_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT801_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

extern GsIMAGE gModelVariant801_aImages[];
extern ModelVariantFlightConfig gModelVariant801_aConfigs[];

s32 ModelVariant801_HandleRequest(ModelVariantFlightState *context, s32 command);

#endif

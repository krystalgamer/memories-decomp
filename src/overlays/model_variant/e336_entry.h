#ifndef MEMORIES_DECOMP_MODEL_VARIANT787_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT787_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

extern GsIMAGE gModelVariant787_aImages[];
extern ModelVariantBandSpokeConfig gModelVariant787_aConfigs[];

s32 ModelVariant787_HandleRequest(ModelVariantBandSpokeState *context, s32 command);

#endif

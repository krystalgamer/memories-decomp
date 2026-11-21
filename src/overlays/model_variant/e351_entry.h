#ifndef MEMORIES_DECOMP_MODEL_VARIANT802_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT802_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

extern GsIMAGE gModelVariant802_aImages[];
extern ModelVariantGlideConfig gModelVariant802_aConfigs[];

s32 ModelVariant802_HandleRequest(ModelVariantGlideState *context, s32 command);

#endif

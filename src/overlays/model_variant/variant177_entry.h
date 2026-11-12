#ifndef MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

extern GsIMAGE gModelVariant177_aImages[];
extern ModelVariantBeamParticleConfig gModelVariant177_aConfigs[];

s32 ModelVariant177_HandleRequest(u8 *context, s32 command);

#endif

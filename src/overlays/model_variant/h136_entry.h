#ifndef FRENCH_MODEL_VARIANT153_ENTRY_H
#define FRENCH_MODEL_VARIANT153_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

extern const VECTOR gFrenchModelVariant153_UnitScale;
extern GsIMAGE gFrenchModelVariant153_aImages[];
extern ModelVariantRisingBurstConfig gFrenchModelVariant153_aConfigs[];
s32 FrenchModelVariant153_HandleRequest(ModelVariantRisingBurstState *context, s32 command);

#endif

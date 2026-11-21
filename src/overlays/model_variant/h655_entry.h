#ifndef MEMORIES_DECOMP_MODEL_VARIANT655_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT655_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant655_aImages[2];
/* One 0xC-byte command row: the start delay, the size of the quads and
 * rings, then the streak colour and the burst/spark colour. */
extern ModelVariantTwoColorCommand gModelVariant655_aConfigs[];

s32 ModelVariant655_HandleRequest(ModelVariantStreakBurstState *context, s32 command);

#endif

#ifndef MEMORIES_MODEL_VARIANT409_ENTRY_H
#define MEMORIES_MODEL_VARIANT409_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "model_variant.h"
#include "../french_model_variant/variant426_entry_pal.h"

/* The US header-409 entry builds the same effect as the French header-426
 * entry over the same record. */
/* Nine GsIMAGE records (0x1C bytes each) followed by the command table. */
extern GsIMAGE gModelVariant409_aImages[9];
extern ModelVariantSpiralConfig gModelVariant409_aConfigs[];

#endif

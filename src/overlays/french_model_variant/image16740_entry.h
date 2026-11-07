#ifndef FRENCH_MODEL_IMAGE16740_ENTRY_H
#define FRENCH_MODEL_IMAGE16740_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"

typedef struct {
    ModelVariantBeamRingConfig *config;
    SVECTOR beams[32];
    SVECTOR origin;
    SVECTOR ring[66];
    SVECTOR translation;
    s32 textures[2];
    s32 time;
    u8 completed;
} Image16740State;

extern VECTOR gFrenchModelVariant196_UnitScale;
extern GsIMAGE gFrenchModelVariant196_aImages[2];
extern ModelVariantBeamRingConfig gFrenchModelVariant196_aConfigs[];

#endif

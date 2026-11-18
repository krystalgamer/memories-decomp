#ifndef FRENCH_MODEL_IMAGE26952_ENTRY_H
#define FRENCH_MODEL_IMAGE26952_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/graphics_packet_submission.h"

typedef struct {
    ModelVariantFlightConfig *config;
    SVECTOR positions[8];
    SVECTOR velocities[8];
    SVECTOR rotations[8];
    SVECTOR targets[8];
    SVECTOR history[8][32];
    SVECTOR sparks[8][8];
    u32 elapsed;
    u16 spark_frames[8][8];
    u16 stage;
    u16 hit[8];
    u16 visible, launched;
    u16 length[8];
    u16 head[8];
    s32 ambient;
    s32 textures[2];
    u8 color[8];
} Image26952State;

typedef union {
    SVECTOR vector;
    ModelEffectAdjustment dimensions;
} Image26952Position;

extern VECTOR gFrenchModelVariant818_UnitScale;
extern GsIMAGE gFrenchModelVariant818_aImages[2];
extern ModelVariantFlightConfig gFrenchModelVariant818_aConfigs[];

#endif

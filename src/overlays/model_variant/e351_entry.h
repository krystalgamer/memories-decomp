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

/* The work area: the command row, the slot offset and its per-frame
 * velocity, the particle positions, the slot scale, the elapsed time, the
 * phase, the live particle count, the particle rise speeds and ages, the
 * ambient level and the particle texture. */
typedef struct {
    ModelVariantGlideConfig *config;
    VECTOR offset;
    VECTOR velocity;
    SVECTOR particles[128];
    u32 scale;
    u32 elapsed;
    u16 phase;
    u16 count;
    u16 speeds[128];
    u16 ages[128];
    s32 level;
    s32 texture[1];
} Model802State;

extern GsIMAGE gModelVariant802_aImages[];
extern ModelVariantGlideConfig gModelVariant802_aConfigs[];

s32 ModelVariant802_HandleRequest(Model802State *context, s32 command);

#endif

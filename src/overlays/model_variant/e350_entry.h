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

/* The work area: the command row; per sprite its position, step, angle,
 * target, 32-point trail and sparks; the timer, the spark frames, the
 * phase, the arrival flags, the drawn and moving counts, the trail lengths
 * and heads, the ambient level, the two textures and the sprite levels. */
typedef struct {
    ModelVariantFlightConfig *config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR target[8];
    SVECTOR trail[8][32];
    SVECTOR sparks[8][8];
    u32 elapsed;
    u16 spark_frame[8][8];
    u16 phase;
    u16 arrived[8];
    u16 shown;
    u16 moving;
    u16 tail[8];
    u16 trail_head[8];
    s32 ambient;
    s32 texture[2];
    u8 level[8];
} Model801State;

extern GsIMAGE gModelVariant801_aImages[];
extern ModelVariantFlightConfig gModelVariant801_aConfigs[];

s32 ModelVariant801_HandleRequest(Model801State *context, s32 command);

#endif

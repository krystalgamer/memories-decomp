#ifndef MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

/* The 0x18C-byte work area: the command row, the fourteen ring points, the
 * origin next to the other slot, 32 particles (the padding halfword is the
 * particle's animation frame, or -1 once it is spent), two texture words, the
 * elapsed time, the command group (command / 100) and the completion flag. */
typedef struct {
    ModelVariantBeamParticleConfig *config;
    SVECTOR ring[14];
    SVECTOR origin;
    SVECTOR particles[32];
    s32 texture[2];
    s32 elapsed;
    u8 command_group, completed, unknown_18A[2];
} Model177State;

extern GsIMAGE gModelVariant177_aImages[];
extern ModelVariantBeamParticleConfig gModelVariant177_aConfigs[];

s32 ModelVariant177_HandleRequest(u8 *context, s32 command);

#endif

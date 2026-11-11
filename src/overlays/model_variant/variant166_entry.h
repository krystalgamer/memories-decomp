#ifndef MEMORIES_DECOMP_MODEL_VARIANT166_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT166_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* The 0x63C-byte work area: the command row, 64 particle positions and 64
 * target points, the gather centre, a 66-point double ring, five texture
 * words, the completion flag, the elapsed time and a frame count. */
typedef struct {
    ModelVariantGatherConfig *config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    SVECTOR centre;
    SVECTOR ring[66];
    u32 texture[5];
    u8 completed, unknown_631[3];
    s32 elapsed;
    s32 frames;
} Model166State;

extern GsIMAGE gModelVariant166_aImages[];
extern ModelVariantGatherConfig gModelVariant166_aConfigs[];

s32 ModelVariant166_HandleRequest(u8 *context, s32 command);

#endif

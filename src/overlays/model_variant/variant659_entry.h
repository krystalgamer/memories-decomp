#ifndef MEMORIES_DECOMP_MODEL_VARIANT659_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT659_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: an animated flare, a growing ring or a spark. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    /* Two rings of nine points: [point][ring]. */
    SVECTOR inner[9][2];
    SVECTOR outer[9][2];
    SVECTOR quad[4];
    /* Flare cursor, flare timer and, in [4], the finished latch. */
    u8 flags[5];
    u8 count, unknown_1E2[2];
    ModelVariantElement flares[32];
    ModelVariantElement rings[32];
    ModelVariantElement sparks[32];
} Model659State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant659_aImages[7];
/* One 0xC-byte command row: the start delay, the size of the rings, then
 * the flare colour and the ring/spark colour. */
extern ModelVariantTwoColorCommand gModelVariant659_aConfigs[];

s32 ModelVariant659_HandleRequest(u8 *context, s32 command);

#endif

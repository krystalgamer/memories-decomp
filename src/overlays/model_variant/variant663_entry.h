#ifndef MEMORIES_DECOMP_MODEL_VARIANT663_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT663_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/abs.h"

/* ModelVariantElement here: a strip column or a ray burst. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 tri;
    SVECTOR spokes[17][2];
    SVECTOR rays[33][2];
    SVECTOR fan[17][2];
    u8 flags[5];
    u8 column_count, burst_count, unknown_4CF;
    /* The model-point matrix captured when each strip column spawns. */
    MATRIX matrices[64];
    ModelVariantElement columns[64];
    ModelVariantElement bursts[8];
} Model663State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant663_aImages[2];
/* One 0xC-byte command row: the start delay, the size of the strips and
 * rays, then the strip colour and the ray/fan colour. */
extern ModelVariantTwoColorCommand gModelVariant663_aConfigs[];

s32 ModelVariant663_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT682US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT682US_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
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
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 tri;
    SVECTOR spokes[17][2];
    SVECTOR rays[33][2];
    SVECTOR fan[17][2];
    /* The square every spark sprite is projected from. */
    SVECTOR quad[4];
    u8 flags[5];
    u8 column_count, burst_count, spark_columns, spark_rows, unknown_4F1[3];
    /* The model-point matrix captured when each strip column spawns. */
    MATRIX matrices[64];
    ModelVariantElement columns[64];
    ModelVariantElement bursts[8];
    ModelVariantSpark sparks[16][6];
} Model682UsState;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant682Us_aImages[3];
/* One 0xC-byte command row: the start delay, the size of the strips and
 * rays, then the strip colour and the ray/fan/spark colour. */
extern ModelVariantTwoColorCommand gModelVariant682Us_aConfigs[];

s32 ModelVariant682Us_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT681_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT681_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: a flash, a ring, a burst, a dome or a spark. In
 * rotation, flashes count their frames in rotation.vx. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[6];
    u16 clut[6], unknown_2A;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    SVECTOR ring[17];
    SVECTOR rim[17];
    SVECTOR inner[6][8];
    SVECTOR outer[6][8];
    SVECTOR burst_corners[4];
    SVECTOR dome[17][9];
    SVECTOR cone_inner[9];
    SVECTOR cone_outer[9];
    SVECTOR cone_middle[9];
    SVECTOR spark_corners[4];
    /* [0] rings started, [1] spark row cursor, [2] and [3] spark timers,
     * [5] dome started. */
    u8 flags[6];
    u8 flash_count, column_count, row_count, spark_columns, spark_rows;
    u8 unknown_AA3;
    VECTOR offsets[5];
    ModelVariantElement flashes[2];
    ModelVariantElement rings[4][5];
    ModelVariantElement bursts[4][5];
    ModelVariantElement domes[4];
    ModelVariantElement sparks[32][2];
} Model681State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant681_aImages[8];
/* One 0x10-byte command row: the start delay, the size of the flash and the
 * height of the rings, then the flash, ring/spark and dome colours. */
extern ModelVariantThreeColorCommand gModelVariant681_aConfigs[];

s32 ModelVariant681_HandleRequest(u8 *context, s32 command);

#endif

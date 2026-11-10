#ifndef MEMORIES_DECOMP_MODEL_VARIANT662_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT662_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/abs.h"

/* ModelVariantOffsetElement here: a tube, a cone, a dome or a spark. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    /* [ring point][segment][tube] */
    SVECTOR tube_points[9][17][3];
    /* [ring point][band][cone] */
    SVECTOR cone_points[17][9][3];
    /* [ring point][band][dome] */
    SVECTOR dome_points[9][5][3];
    SVECTOR corners[4];
    u8 flags[5];
    u8 tube_count, spark_count;
    /* Set once a tube has opened its cone and sparks. */
    u8 triggered[3], unknown_218E[2];
    ModelVariantOffsetElement tubes[3];
    ModelVariantOffsetElement cones[3];
    ModelVariantOffsetElement domes[3];
    /* [spark][tube] */
    ModelVariantOffsetElement sparks[8][3];
} Model662State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant662_aImages[3];
/* One 0x10-byte command row: the start delay, the radius and the cone
 * height, then the tube, the cone/dome and the spark colours. */
extern ModelVariantThreeColorCommand gModelVariant662_aConfigs[];

s32 ModelVariant662_HandleRequest(u8 *context, s32 command);

#endif

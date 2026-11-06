#ifndef MEMORIES_DECOMP_MODEL_VARIANT680US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT680US_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: a bloom, a ring or a burst. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4], unknown_22;
    POLY_FT4 quad;
    POLY_GT4 strip;
    /* Five rings of five petal points; each point is doubled into an
     * upper and a lower copy. */
    SVECTOR petals[5][5][2];
    /* The bloom halo square. */
    SVECTOR flare[4];
    /* Six points round an inner and an outer circle, doubled likewise. */
    SVECTOR rim[2][6][2];
    /* The ring, burst and spark squares. */
    SVECTOR squares[3][4];
    /* Bloom cursor, spark-row cursor, end latch, unused, halfway latch. */
    u8 flags[5];
    u8 count, rows, cols;
    u8 joints[4];
    u8 frame_u[8], frame_v[8];
    /* Five flight offsets for each ring. */
    VECTOR cone[5][32];
    /* Five spoke offsets shared by the bursts. */
    VECTOR spokes[5];
    ModelVariantElement blooms[32];
    ModelVariantElement rings[32];
    ModelVariantElement bursts[32];
    ModelVariantSpark sparks[8][6];
} Model680UsState;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant680Us_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant680Us_aImages[11];
/* One 0xC-byte command row: the start delay, the effect radius, the
 * flight height and two colours (blooms and sparks, then rings and
 * bursts). */
extern ModelVariantTwoColorCommand gModelVariant680Us_aConfigs[];

s32 ModelVariant680Us_HandleRequest(u8 *context, s32 command);

#endif

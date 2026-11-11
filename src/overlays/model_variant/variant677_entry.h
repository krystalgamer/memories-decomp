#ifndef MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H

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

/* ModelVariantElement here: a spark, a ring or a burst. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4], unknown_22;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    SVECTOR ring_corners[4];
    SVECTOR burst_corners[4];
    SVECTOR ring_inner[17];
    SVECTOR ring_outer[17];
    SVECTOR ring_middle[17];
    SVECTOR wave_corners[4];
    SVECTOR cone_inner[17];
    SVECTOR cone_outer[17];
    SVECTOR cone_middle[17];
    /* [0] spark row cursor, [4] the finished latch. */
    u8 flags[5];
    u8 column_count, row_count;
    /* Texture-cell origins indexed by a ring's u. */
    u8 cell_u[8];
    u8 cell_v[8];
    u8 unknown_447;
    VECTOR offsets[5];
    VECTOR spread[5];
    ModelVariantElement sparks[12][2];
    ModelVariantElement rings[12][2];
    ModelVariantElement bursts[12][2];
} Model677State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant677_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant677_aImages[11];
/* One 0xC-byte command row: the start delay, the effect radius, an unused
 * height, then the spark colour and the ring/burst colour. */
extern ModelVariantTwoColorCommand gModelVariant677_aConfigs[];

s32 ModelVariant677_HandleRequest(u8 *context, s32 command);

#endif

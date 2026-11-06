#ifndef MEMORIES_DECOMP_MODEL_VARIANT661_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT661_ENTRY_H

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

/* ModelVariantCountedElement here: a streak, a burst or a spark. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_G4 quad;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Two edges of each streak's trail: [edge][point][streak]. */
    SVECTOR trail[2][64][8];
    SVECTOR offset;
    SVECTOR prev[2];
    SVECTOR heads[2][8];
    SVECTOR corners[8];
    SVECTOR ring[2][16];
    SVECTOR outer[2][16];
    u8 flags[5];
    u8 rows, cols, burst_count, spark_count, unknown_2381[3];
    ModelVariantCountedElement streaks[8];
    ModelVariantCountedElement bursts[8];
    ModelVariantCountedElement sparks[8][8];
} Model661State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant661_aImages[2];
/* One 0xC-byte command row: the start delay, the size of the quads and
 * rings, then the streak colour and the burst/spark colour. */
extern ModelVariantTwoColorCommand gModelVariant661_aConfigs[];

s32 ModelVariant661_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT652_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT652_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantSpinElement here: a ring, a dome, a spark or the screen flash.
 * In scale, the sparks keep their outward drift here instead of a scale. In
 * spin, the domes keep their row count in vx; the sparks their rise. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s32 other_slot;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_22;
    POLY_GT3 fan;
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_F4 flash_poly;
    /* Seventeen points round the ring and, in [1], their rotated copy. */
    SVECTOR rim[17][2];
    /* Seventeen meridians of nine points on the dome. */
    SVECTOR dome[17][9];
    SVECTOR corners[4];
    /* The ring hand-over latch in flags[0]. */
    u8 flags[5];
    u8 ring_count, dome_count, spark_count;
    u8 rim_uv[17][2];
    /* Per-point red, green and blue dome shading. */
    u8 shade[3][17][9];
    s16 cursor;
    SVECTOR origin;
    ModelVariantSpinElement rings[64];
    ModelVariantSpinElement domes[4];
    ModelVariantSpinElement sparks[3][8][4];
    ModelVariantSpinElement flash;
} Model652State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant652_aImages[3];
/* One 0xC-byte command row: the start delay, the ring radius, the dome
 * height, then the ring/spark colour and the dome colour. */
extern ModelVariantTwoColorCommand gModelVariant652_aConfigs[];

s32 ModelVariant652_HandleRequest(u8 *context, s32 command);

#endif

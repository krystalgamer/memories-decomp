#ifndef MEMORIES_DECOMP_MODEL_VARIANT646_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT646_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: a glow, shard, orb, band ring or ray sphere
 * placed through its own coordinate unit. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_FT4 sprite;
    POLY_GT3 shard;
    POLY_GT4 strip;
    GsGLINE line;
    SVECTOR corners[4];
    SVECTOR triangle[3];
    /* Inner and outer ring points, each in the XY and the XZ plane. */
    SVECTOR ring_points[17][2][2];
    /* Eight meridians of nine points on a sphere; the last row is unused. */
    SVECTOR sphere[9][9];
    /* Orb cursor, then the restore request at flags[4]. */
    u8 flags[5];
    u8 copies, orb_count, shard_count;
    /* Per-orb numerator and divisor of the X and Y arcs. */
    s32 arc_x[8][2];
    s32 arc_y[8][2];
    ModelVariantElement glows[2];
    ModelVariantElement shards[16][2];
    ModelVariantElement orbs[8][2];
    ModelVariantElement rings[8][2];
    ModelVariantElement rays[8][2];
} Model646State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant646_aImages[2];
/* One 0xC-byte command row: the start delay, the size of the glows and
 * shells, then the glow/shard colour and the band/ray colour. */
extern ModelVariantTwoColorCommand gModelVariant646_aConfigs[];

s32 ModelVariant646_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT660US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT660US_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantOffsetElement here: a glow, shard, orb, band ring, ray sphere
 * or spark placed through its own coordinate unit. In offset, a spark's
 * final offset from its orb; zero for the other elements. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    ModelVariantEntryPlacement place;
    ModelVariantTextureIds textures;
    u16 unknown_1E;
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
    /* Corners of the small spark quad, a quarter of the radius. */
    SVECTOR spark_corners[4];
    /* Orb cursors of the two copies, then the restore request at flags[4]. */
    u8 flags[5];
    u8 copies, orb_count, shard_count, spark_count;
    /* Per-orb numerator and divisor of the X and Y arcs. */
    s32 arc_x[8][2];
    s32 arc_y[8][2];
    ModelVariantOffsetElement glows[2];
    ModelVariantOffsetElement shards[16][2];
    ModelVariantOffsetElement orbs[8][2];
    ModelVariantOffsetElement rings[8][2];
    ModelVariantOffsetElement rays[8][2];
    /* Five sparks per orb and copy. */
    ModelVariantOffsetElement sparks[5][8][2];
} Model660UsState;

extern MATRIX GsIDMATRIX;
/* Spark, shard/band and glow/orb texture images; the entry uploads them last
 * to first, so tpage[0] belongs to the glows and orbs. */
extern GsIMAGE gModelVariant660Us_aImages[3];
/* One 0xC-byte command row: the start delay, the size of the glows and
 * shards, then the glow/shard/orb colour and the band/ray/spark colour. */
extern ModelVariantTwoColorCommand gModelVariant660Us_aConfigs[];

s32 ModelVariant660Us_HandleRequest(u8 *context, s32 command);

#endif

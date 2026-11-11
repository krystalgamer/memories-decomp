#ifndef MEMORIES_DECOMP_MODEL_VARIANT678US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT678US_ENTRY_H

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

/* ModelVariantElement here: a rising bubble, a burst or a spray, placed
 * through its own coordinate unit. In target, a bubble's rise: the arc
 * height in vy and the end depth in vz. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4];
    u16 unknown_22;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Bubble, burst and spray squares of one, two and half the radius. */
    SVECTOR bubble_corners[4];
    SVECTOR burst_corners[4];
    SVECTOR spray_corners[4];
    /* Inner and outer rings of seventeen points and their midpoints. */
    SVECTOR inner_ring[17];
    SVECTOR outer_ring[17];
    SVECTOR middle_ring[17];
    SVECTOR shard_corners[4];
    /* Bubble cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 rows, columns;
    /* Texture-sheet cells of the burst animation. */
    u8 cell_u[8];
    u8 cell_v[8];
    /* Spray directions, then shard directions, both rotated from the slot. */
    VECTOR spray_axes[5];
    VECTOR shard_axes[16];
    ModelVariantElement bubbles[12][2];
    ModelVariantElement bursts[12][2];
    ModelVariantElement sprays[12][2];
} Model678UsState;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant678Us_aImages[11];
/* One 0xC-byte command row: the start delay, the radius and the rise height,
 * then the bubble colour and the burst/spray colour. */
extern ModelVariantTwoColorCommand gModelVariant678Us_aConfigs[];

s32 ModelVariant678Us_HandleRequest(u8 *context, s32 command);

#endif

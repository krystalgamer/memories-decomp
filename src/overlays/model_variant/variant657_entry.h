#ifndef MEMORIES_DECOMP_MODEL_VARIANT657_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT657_ENTRY_H

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

/* ModelVariantOffsetElement here: an effect part placed through a
 * camera-relative coordinate unit; the shards also keep an offset from their
 * ring's centre. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    ModelVariantEntryPlacement place;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    SVECTOR ring[9][17][3];
    SVECTOR band[17][9][3];
    SVECTOR cap[9][5][3];
    SVECTOR corners[4];
    /* Flag 1 marks a started effect and flag 4 a finished one. */
    u8 flags[5];
    u8 ring_count, shard_count;
    u8 triggered[3], unknown_218E[2];
    ModelVariantOffsetElement rings[3];
    ModelVariantOffsetElement bands[3];
    ModelVariantOffsetElement caps[3];
    ModelVariantOffsetElement shards[8][3];
} Model657State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant657_aImages[3];
/* One 0x10-byte command row: the start delay, the ring radius, the band
 * height, then the ring, the band/cap and the shard colours. */
extern ModelVariantThreeColorCommand gModelVariant657_aConfigs[];

s32 ModelVariant657_HandleRequest(u8 *context, s32 command);

#endif

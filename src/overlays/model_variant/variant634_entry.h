#ifndef MEMORIES_DECOMP_MODEL_VARIANT634_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT634_ENTRY_H

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

/* ModelVariantBurst here: one ring or one spark: the 0x4C-byte burst record
 * of header 647. */

/* The work area: the command, the radius, the two textures, the shared band
 * and quad packets, the band vertices and the quad corners, the flags, the
 * ring and spark counts, and up to 32 rings and 32 sparks. */
typedef struct {
    ModelVariantEntryHead head;
    s16 delay;
    u16 radius, unknown_0C;
    u16 tpage[2], unknown_12;
    u16 clut[2], unknown_18[2];
    POLY_GT4 quad;
    POLY_FT4 flat;
    SVECTOR ring[17][2];
    SVECTOR square[4];
    u8 flags[5];
    u8 ring_count, spark_count, unknown_1AF;
    ModelVariantBurst rings[32];
    ModelVariantBurst sparks[32];
} Model634State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant634_aImages[2];
/* One 0xC-byte command row: the start delay, the radius, a value the entry
 * copies but never reads, the ring colour and the spark colour. */
extern ModelVariantSparkCommand gModelVariant634_aConfigs[];

s32 ModelVariant634_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT645_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT645_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: a tube puff left at a model joint, or a halo
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
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 fan;
    /* The tube's two end rings, sixteen segments around. */
    SVECTOR tube[17][2];
    /* The halo's inner and outer band, 32 segments around. */
    SVECTOR band[33][2];
    /* The halo's spinning blades: inner and outer points, 16 around. */
    SVECTOR blades[17][2];
    /* Puff cursor, then the halo stage. */
    u8 flags[5];
    u8 puff_count, halo_count, unknown_4CF;
    /* The joint matrix each puff was left with. */
    MATRIX joints[64];
    ModelVariantElement puffs[64];
    ModelVariantElement halos[8];
} Model645State;

/* A blade corner and its rotated copy. */
typedef struct {
    SVECTOR in, out;
} Model645Corner;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant645_aImages[2];
/* One 0xC-byte command row: the start delay, the tube radius and length,
 * then the tube colour and the halo colour. */
extern ModelVariantTwoColorCommand gModelVariant645_aConfigs[];

s32 ModelVariant645_HandleRequest(u8 *context, s32 command);

#endif

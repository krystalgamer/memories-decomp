#ifndef MEMORIES_DECOMP_MODEL_VARIANT649US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT649US_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* ModelVariantBurst here: an element placed through its own coordinate unit. */

/* The work area: the command, the slot, the packets, the meshes and the
 * element arrays. The petals are textured quads strung between two joints,
 * the dome is an 8x8 textured grid over a hemisphere, the card is a
 * textured square flying from its anchor to the joints' midpoint, the
 * shards are textured triangles following it and the sparks are pairs of
 * thin triangles bursting from where the card lands. */
typedef struct {
    ModelVariantEntryHead head;
    s32 side;
    s32 other;
    s32 tint;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4];
    u16 unknown_2A;
    POLY_GT4 quad;
    POLY_FT4 card;
    POLY_GT3 shard;
    SVECTOR petal[4];
    SVECTOR dome[9][9];
    SVECTOR square[4];
    SVECTOR tri[3];
    SVECTOR spike[4];
    /* The camera latch in [0] and, in [4], the finished frame counter. */
    u8 flags[5];
    u8 petal_count, shard_count, spark_count;
    SVECTOR anchor;
    ModelVariantBurst petals[64];
    ModelVariantBurst dome_element;
    ModelVariantBurst card_element;
    ModelVariantBurst shards[32];
    ModelVariantBurst sparks[32];
} Model649UsState;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant649Us_aImages[];
/* One 0x0C-byte command row: the start delay, the radius, the height, then
 * the primary and the secondary colour. */
extern ModelVariantTwoColorCommand gModelVariant649Us_aConfigs[];

s32 ModelVariant649Us_HandleRequest(u8 *context, s32 command);

#endif

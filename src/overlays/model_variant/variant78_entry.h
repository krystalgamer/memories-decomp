#ifndef MEMORIES_DECOMP_MODEL_VARIANT78_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT78_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One grid vertex colour. */
typedef struct {
    u8 red, green, blue, unknown_03;
} Model78Colour;

/* The 0xB48-byte work area: the command row, the 13x13 wave grid and its
 * colours, the ring's lower and upper edges, 34 orbiting sprites, 32 rising
 * sprites, the orbiting sprites' frames, the spin angle, three texture words,
 * the completion flag and the elapsed time. */
typedef struct {
    ModelVariantWaveConfig *config;
    SVECTOR grid[169];
    Model78Colour colours[169];
    SVECTOR lower[17];
    SVECTOR upper[17];
    SVECTOR orbit[34];
    SVECTOR rising[32];
    u8 frames[34];
    s16 spin;
    s32 texture[3];
    u8 completed, unknown_B41[3];
    s32 elapsed;
} Model78State;

extern GsIMAGE gModelVariant78_aImages[];
extern ModelVariantWaveConfig gModelVariant78_aConfigs[];

s32 ModelVariant78_HandleRequest(u8 *context, s32 command);

#endif

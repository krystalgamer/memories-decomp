#ifndef MEMORIES_DECOMP_MODEL_VARIANT667_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT667_ENTRY_H

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


/* ModelVariantCountedElement here: a ribbon, a sprite, a fan or a dome. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_G3 fan;
    POLY_G4 quad;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Two edges of each ribbon's trail: [edge][point][ribbon]. */
    SVECTOR trail[2][64][2];
    SVECTOR corners[4];
    SVECTOR prev[2];
    SVECTOR heads[2][2];
    SVECTOR fan_rim[17];
    SVECTOR cap[8];
    SVECTOR ring[17];
    SVECTOR outer[17];
    u8 flags[5];
    u8 rows, cols, sprite_count, fan_count, dome_count;
    u8 cell_u[9], cell_v[9];
    /* Filled by Model_CopySlotPosition; the fourth halfword is unused. */
    s16 origin[4];
    ModelVariantCountedElement ribbons[2];
    ModelVariantCountedElement sprites[16];
    ModelVariantCountedElement fans[16];
    ModelVariantCountedElement domes[32];
} Model667State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant667_aImages[3];
/* One 0xC-byte command row: the start delay, the quad and dome sizes, then
 * the ribbon colour and the sprite colour. */
extern ModelVariantTwoColorCommand gModelVariant667_aConfigs[];

s32 ModelVariant667_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT647_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT647_ENTRY_H

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

/* ModelVariantBurst here: one burst: a cone of rings that grows from the
 * camera unit while it fades, drawn as quads and triangles from the shared
 * ring vertices. */

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT3 triangle;
    POLY_GT4 quad;
    SVECTOR inner[17][4];
    SVECTOR outer[17][4];
    SVECTOR top[17][8];
    SVECTOR bottom[17][8];
    u8 flags[5];
    u8 count, unknown_D42[2];
    ModelVariantBurst bursts[2];
    u8 unknown_DDC[0x428];
} Model647State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant647_Image;
/* One 0xC-byte command row: the start delay, the ring radius and height,
 * and the colour of the bursts. */
extern ModelVariantOneColorCommand gModelVariant647_aConfigs[];

s32 ModelVariant647_HandleRequest(u8 *context, s32 command);

#endif

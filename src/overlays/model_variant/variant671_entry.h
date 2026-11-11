#ifndef MEMORIES_DECOMP_MODEL_VARIANT671_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT671_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantElement here: a petal bloom or a ring. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 quad;
    POLY_GT3 fan;
    POLY_GT4 strip;
    /* The bloom square. */
    SVECTOR flare[4];
    /* Three cone points for each of eight petals in eight turns. */
    SVECTOR grid[3][8][8];
    /* Seventeen points round an inner and an outer circle. */
    SVECTOR rim[2][17];
    /* The ring square. */
    SVECTOR ring[4];
    /* Spawn cursor and, in [4], the halfway latch. */
    u8 flags[5];
    u8 count;
    SVECTOR origin;
    ModelVariantElement blooms[32];
    ModelVariantElement rings[32];
} Model671State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant671_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant671_aImages[3];
/* One 0xC-byte command row: the start delay, the effect radius, the
 * cone height and two colours (petal blooms, then rings). */
extern ModelVariantTwoColorCommand gModelVariant671_aConfigs[];

s32 ModelVariant671_HandleRequest(u8 *context, s32 command);

#endif

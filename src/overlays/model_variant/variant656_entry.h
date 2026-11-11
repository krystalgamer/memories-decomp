#ifndef MEMORIES_DECOMP_MODEL_VARIANT656_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT656_ENTRY_H

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

/* ModelVariantGrowthElement here: a sprite, cone or dome placed through its
 * own coordinate unit. */

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_FT4 sprite;
    POLY_GT4 strip;
    u32 unknown_7C[5];
    SVECTOR corners[4];
    /* Nine rings of eight points widening along a cone. */
    SVECTOR cone[9][8];
    SVECTOR rim[9];
    SVECTOR inner[9];
    SVECTOR outer[9];
    /* Nine rows of a dome's inner and outer points. */
    SVECTOR dome_inner[9][8];
    SVECTOR dome_outer[9][8];
    /* Spawn cursor, then the finished latch at flags[4]. */
    u8 flags[5];
    u8 count, dome_count, unknown_84F;
    MATRIX joints[16];
    ModelVariantGrowthElement sprites[16];
    ModelVariantGrowthElement cones[16];
    ModelVariantGrowthElement domes[16];
} Model656State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4 through the halfword view. */
typedef struct {
    union {
        u8 bytes[8];
        s16 halves[4];
    } u;
} Model656ViewSnapshot;

/* The work-size string; slot-1 images carry a nonzero pad byte after it. */
extern char gModelVariant656_szWorkSizeFormat[];
extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant656_aImages[2];
/* One 0xC-byte command row: the start delay, the size and height of the
 * effect, then the ring/shell colour and the sprite colour. */
extern ModelVariantTwoColorCommand gModelVariant656_aConfigs[];

s32 ModelVariant656_HandleRequest(u8 *context, s32 command);

#endif

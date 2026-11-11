#ifndef MEMORIES_DECOMP_MODEL_VARIANT672_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT672_ENTRY_H

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

/* One 0x50-byte element: a spark streak, a flare or a ring sprite. */
typedef struct {
    ModelVariantElementHead head;
    u16 spawned, unknown_12;
    ModelVariantElementTransform xform;
    VECTOR target;
} Model672Element;

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 quad;
    POLY_GT3 fan;
    GsGLINE line;
    SVECTOR center;
    /* Seventeen points round a circle of the command's radius. */
    SVECTOR rim[17];
    /* The flare square and the ring square. */
    SVECTOR flare[4];
    SVECTOR ring[4];
    SVECTOR origin;
    /* Spark cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count;
    Model672Element sparks[64];
    Model672Element flares[64];
    Model672Element rings[64];
} Model672State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant672_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant672_aImages[3];
/* One 0xC-byte command row: the start delay, the spark spread, the
 * flare size and two colours (sparks and rings, then flares). */
extern ModelVariantTwoColorCommand gModelVariant672_aConfigs[];

s32 ModelVariant672_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT666_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT666_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libgte.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x54-byte element: a bloom, a spark or a ring. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Bloom frame counter, spark cursor and spark ring latch. */
    u16 frame, cursor, spawned, unknown_16;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model666Element;

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 quad;
    POLY_FT3 fan;
    GsGLINE line;
    /* The bloom square. */
    SVECTOR flare[4];
    /* The spark root and nine points round a quarter circle. */
    SVECTOR root;
    SVECTOR rim[9];
    /* The ring square. */
    SVECTOR ring[4];
    /* Flag [4] latches once the finale has been signalled. */
    u8 flags[5];
    u8 count, unknown_112, sparks_per_bloom;
    SVECTOR origin;
    Model666Element blooms[3];
    Model666Element sparks[16][3];
    Model666Element rings[16][3];
} Model666State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant666_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant666_aImages[3];
/* One 0xC-byte command row: the start delay, the bloom radius, the
 * spark height and two colours (blooms and sparks, then rings). */
extern ModelVariantTwoColorCommand gModelVariant666_aConfigs[];

s32 ModelVariant666_HandleRequest(u8 *context, s32 command);

#endif

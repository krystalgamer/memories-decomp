#ifndef MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x50-byte element: a cone ring, the flat band or a grid sprite. */
typedef struct {
    ModelVariantElementHead head;
    /* Next grid row to spawn; only the first row's copy is used. */
    u8 cursor, unknown_11[3];
    ModelVariantElementTransform xform;
    VECTOR target;
} Model643Element;

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    ModelVariantEntryPlacement place;
    ModelVariantTextureIds textures;
    u16 unknown_1E;
    POLY_GT4 strip;
    POLY_G4 band;
    POLY_FT4 quad;
    /* Seventeen inner and outer points round the cone. */
    SVECTOR cone[17][2];
    /* Seventeen inner and outer points round the flat band. */
    SVECTOR ring[17][2];
    SVECTOR corner[4];
    /* [0] latches the grid start, [4] the finished latch. */
    u8 flags[5];
    u8 cone_count, band_count, rows, columns;
    Model643Element cones[4];
    Model643Element bands[1];
    /* [row][column] */
    Model643Element grid[8][16];
} Model643State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant643_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant643_aImages[2];
/* One 0xC-byte command row: the start delay, the ring radius, the
 * cone height and two colours (rings and band, then the sprite grid). */
extern ModelVariantTwoColorCommand gModelVariant643_aConfigs[];

s32 ModelVariant643_HandleRequest(u8 *context, s32 command);

#endif

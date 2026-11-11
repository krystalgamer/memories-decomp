#ifndef MEMORIES_DECOMP_MODEL_VARIANT668_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT668_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the shell radius, the angle
 * added to every petal rotation, then the shell colour and the flash colour. */
typedef struct {
    s16 delay;
    u16 radius, twist;
    u8 color[6];
} Model668Command;

/* One 0x50-byte element: the shell, or the screen flash. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Frames drawn; odd frames shrink the shell slightly. */
    u16 frame, unknown_12;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR spin;
} Model668Element;

typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, twist;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_F4 flash_poly;
    /* Two edges of eight petals of eight bands on nine rings. */
    SVECTOR grid[2][9][8][8];
    SVECTOR corners[4];
    /* flags[4] is the restore request. */
    u8 flags[5];
    u8 shell_count, unknown_24BA[2];
    Model668Element shells[8];
    Model668Element flash;
} Model668State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant668_Image;
extern Model668Command gModelVariant668_aConfigs[];

s32 ModelVariant668_HandleRequest(u8 *context, s32 command);

#endif

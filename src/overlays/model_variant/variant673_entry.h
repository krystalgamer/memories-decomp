#ifndef MEMORIES_DECOMP_MODEL_VARIANT673_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT673_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the spread radius, the rise
 * height, the quad colour and the fan colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, fan_red, fan_green, fan_blue;
} Model673Command;

/* ModelVariantBurst here: a quad record flying from position along growth
 * over its lifetime. */

/* The work area: the command, the slot, the packets, the quad corners, the
 * fan ring and its centre and corners, the slot position, the flags, and
 * three record arrays: the rising quads, the falling quads and the fans. */
typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_FT4 flat;
    POLY_GT3 fan;
    SVECTOR square[4];
    SVECTOR ring[17];
    SVECTOR centre;
    SVECTOR corners[4];
    s16 position[3];
    u16 unknown_146;
    u8 flags[5];
    u8 count;
    u8 unknown_14E[2];
    ModelVariantBurst rising[64];
    ModelVariantBurst falling[64];
    ModelVariantBurst fans[64];
} Model673State;

extern MATRIX GsIDMATRIX;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant673_szWorkSizeFormat[];
#endif
extern GsIMAGE gModelVariant673_aImages[3];
extern Model673Command gModelVariant673_aConfigs[];

s32 ModelVariant673_HandleRequest(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT674_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT674_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the ring radius, the ring
 * height, the ring colour and the dome colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, dome_red, dome_green, dome_blue;
} Model674Command;

/* ModelVariantTickedBurst here: an effect record: a ring of strips or a dome
 * at position. */

/* The work area: the command, the slot, the packets, the flash square, the
 * upper, lower and middle ring outlines, the dome grid (17 meridians of 9
 * points), the inner, outer and middle cone outlines, the flags, and the
 * ring and dome records. */
typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_FT4 flat;
    POLY_GT4 shaded;
    SVECTOR square[4];
    SVECTOR ring_upper[17];
    SVECTOR ring_lower[17];
    SVECTOR ring_middle[17];
    SVECTOR dome[17][9];
    SVECTOR cone_inner[9];
    SVECTOR cone_outer[9];
    SVECTOR cone_middle[9];
    u8 flags[5];
    u8 count;
    u8 unknown_7DA[2];
    ModelVariantTickedBurst rings[3];
    ModelVariantTickedBurst domes[3];
} Model674State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant674_aImages[5];
extern Model674Command gModelVariant674_aConfigs[];

s32 ModelVariant674_HandleRequest(u8 *context, s32 command);

#endif

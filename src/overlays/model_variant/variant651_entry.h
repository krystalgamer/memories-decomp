#ifndef MEMORIES_DECOMP_MODEL_VARIANT651_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT651_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* ModelVariantBurst here: a strip record. */

/* The work area: the command, the slot, the shaded quad, the inner ring,
 * the dome rows, the outer ring, the flags, the dome row timers and three
 * strip arrays: the spinning rings, the domes and the rising cones. */
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
    POLY_GT4 quad;
    SVECTOR ring[17][2];
    SVECTOR dome[17][2][8];
    SVECTOR outer[17][2];
    u8 flags[5];
    u8 ring_count, dome_count, unknown_AFB;
    u16 row_timer[8];
    ModelVariantBurst rings[8];
    ModelVariantBurst domes[4];
    ModelVariantBurst cones[8];
} Model651State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant651_Image;
/* One 0xC-byte command row: the start delay, the spread radius, the rise
 * height and the strip colour. */
extern ModelVariantOneColorCommand gModelVariant651_aConfigs[];

s32 ModelVariant651_HandleRequest(u8 *context, s32 command);

#endif

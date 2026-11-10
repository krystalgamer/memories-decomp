#ifndef MEMORIES_DECOMP_MODEL_VARIANT670_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT670_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the radius, the height, the
 * shell colour and the ring colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, ring_red, ring_green, ring_blue;
} Model670Command;

/* ModelVariantBurst here: a textured shell or ring placed at position. */

/* The work area: the command, the slot, the packets, the meshes and the
 * two element arrays. The shell is a sprite square, an eight-point base in
 * two rings, an 8x4 grid of dome points and the 2x8x4 mesh a fifth of the
 * way from each base ring to the dome; the ring is a band between an inner
 * and an outer circle, with a square flash on top. */
typedef struct {
    ModelVariantEntryHead head;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_FT4 sprite;
    POLY_GT3 fan;
    POLY_GT4 strip;
    SVECTOR square[4];
    /* [ring][point] */
    SVECTOR base[2][8];
    /* [meridian][row] */
    SVECTOR dome[8][4];
    SVECTOR inner[17];
    SVECTOR outer[17];
    SVECTOR corners[4];
    /* [ring][meridian][row] */
    SVECTOR mesh[2][8][4];
    /* Shell cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count;
    s16 position[3];
    u8 unknown_580[4];
    ModelVariantBurst shells[32];
    ModelVariantBurst rings[32];
} Model670State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant670_aImages[3];
extern Model670Command gModelVariant670_aConfigs[];

s32 ModelVariant670_HandleRequest(u8 *context, s32 command);

#endif

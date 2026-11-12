#ifndef MEMORIES_DECOMP_MODEL_VARIANT628_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT628_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xA-byte command row: the start delay, the dome radius and height
 * and the effect colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, unknown_09;
} Model628Command;

/* One 0x50-byte effect part placed through its own coordinate unit. */
typedef struct {
    ModelVariantElementHead head;
    /* The wobble phase of the domes. */
    u16 stage, unknown_12;
    ModelVariantElementTransform xform;
    VECTOR velocity;
} Model628Part;

/* A ring point and the matching point on the wider, lowered outer ring. */
typedef struct {
    SVECTOR inner, outer;
} Model628RingPoint;

/* The work area: the command, the slot, the packets, the sprite square,
 * the dome grid (17 meridians of 9 points) and its spun copy, the burst
 * ring, the flags and counts, and the four part families. */
typedef struct {
    ModelVariantEntryHead head;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1A;
    POLY_FT4 flat;
    POLY_GT4 shaded;
    u8 unknown_78[0x24];
    SVECTOR square[4];
    SVECTOR dome[17][9];
    SVECTOR spun[17][9];
    Model628RingPoint ring[17];
    /* The burst launch cursor, then the finish flag at flags[4]. */
    u8 flags[5];
    u8 count, spark_count, dome_count, burst_count;
    u8 unknown_B65[3];
    Model628Part balls[3];
    Model628Part sparks[8][3];
    Model628Part domes[2][3];
    Model628Part bursts[32][3];
} Model628State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant628_aImages[3];
extern Model628Command gModelVariant628_aConfigs[];

s32 ModelVariant628_HandleRequest(u8 *context, s32 command);

#endif

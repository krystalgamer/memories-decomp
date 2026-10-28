#ifndef MEMORIES_DECOMP_MODEL_VARIANT630_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT630_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xA-byte command row: the start delay, the sprite size, a second
 * size this entry does not use, and the orb colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[3];
} Model630Command;

/* One 0x4C-byte element: an orb, its fading ring or a spark sprite. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model630Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_12;
    u16 clut[2];
    u8 unknown_18[4];
    POLY_FT4 quad;
    POLY_G4 band;
    /* The sprite square. */
    SVECTOR square[4];
    /* Seventeen points round an inner and an outer circle. */
    SVECTOR rim[17][2];
    /* The volley cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count, rows, unknown_19F;
    Model630Element orbs[10];
    Model630Element rings[10];
    Model630Element sparks[10][3];
} Model630State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model630ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013C7C8[2];
extern Model630Command D_8013C800[];

s32 func_8013B004(u8 *context, s32 command);

#endif

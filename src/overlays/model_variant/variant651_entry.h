#ifndef MEMORIES_DECOMP_MODEL_VARIANT651_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT651_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the spread radius, the rise
 * height and the strip colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, unknown_09[3];
} Model651Command;

/* One 0x4C-byte strip record. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model651Strip;

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
    Model651Strip rings[8];
    Model651Strip domes[4];
    Model651Strip cones[8];
} Model651State;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CA4C;
extern Model651Command D_8013CA68[];

s32 func_8013B004(u8 *context, s32 command);

#endif

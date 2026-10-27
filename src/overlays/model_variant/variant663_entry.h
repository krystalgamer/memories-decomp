#ifndef MEMORIES_DECOMP_MODEL_VARIANT663_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT663_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/abs.h"

/* One 0xC-byte command row: the start delay, the size of the strips and
 * rays, then the strip colour and the ray/fan colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model663Command;

/* One 0x4C-byte element: a strip column or a ray burst. */
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
    VECTOR unknown_3C;
} Model663Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 tri;
    SVECTOR spokes[17][2];
    SVECTOR rays[33][2];
    SVECTOR fan[17][2];
    u8 flags[5];
    u8 column_count, burst_count, unknown_4CF;
    /* The model-point matrix captured when each strip column spawns. */
    MATRIX matrices[64];
    Model663Element columns[64];
    Model663Element bursts[8];
} Model663State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model663ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E354[2];
extern Model663Command D_8013E38C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

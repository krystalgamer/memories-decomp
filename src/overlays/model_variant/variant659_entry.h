#ifndef MEMORIES_DECOMP_MODEL_VARIANT659_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT659_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the size of the rings, then
 * the flare colour and the ring/spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model659Command;

/* One 0x4C-byte element: an animated flare, a growing ring or a spark. */
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
} Model659Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    /* Two rings of nine points: [point][ring]. */
    SVECTOR inner[9][2];
    SVECTOR outer[9][2];
    SVECTOR quad[4];
    /* Flare cursor, flare timer and, in [4], the finished latch. */
    u8 flags[5];
    u8 count, unknown_1E2[2];
    Model659Element flares[32];
    Model659Element rings[32];
    Model659Element sparks[32];
} Model659State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model659ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D320[7];
extern Model659Command D_8013D3E4[];

s32 func_8013B004(u8 *context, s32 command);

#endif

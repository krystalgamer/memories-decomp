#ifndef MEMORIES_DECOMP_MODEL_VARIANT648_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT648_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the size of the square and
 * spokes, and the colours of the sprites and of the spoke strips. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model648Command;

/* One 0x54-byte element: a spinning cluster of fading sprites that drift
 * out from the element's position, then a fan of spoke strips over it. */
typedef struct {
    u8 active;
    u8 color[6];
    u8 u, v, unknown_09[3];
    s32 unknown_0C;
    u16 elapsed, duration;
    u8 unknown_14, unknown_15[3];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_44;
} Model648Burst;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    SVECTOR ring[4];
    SVECTOR spokes[17][2];
    u8 flags[5];
    u8 rows, cols, unknown_1D3;
    Model648Burst bursts[2][48];
} Model648State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model648ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CB84[2];
extern Model648Command D_8013CBBC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

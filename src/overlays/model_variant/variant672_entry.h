#ifndef MEMORIES_DECOMP_MODEL_VARIANT672_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT672_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the spread radius and the
 * burst size, then the streak colour and the sprite/ribbon colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model672Command;

/* One 0x50-byte element: a streak, a sprite with its ribbon fan, or a
 * burst, placed through its own coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Streaks only: set once the matching sprite has been released. */
    u16 triggered, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    /* The streak end point; for bursts only vz, the burst radius. */
    VECTOR target;
} Model672Element;

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
    POLY_GT3 ribbon;
    GsGLINE line;
    SVECTOR center;
    /* Sixteen ribbon segments around the sprite. */
    SVECTOR circle[17];
    SVECTOR corners[4];
    SVECTOR burst_corners[4];
    /* The opponent-side point the streaks spread around. */
    SVECTOR target;
    /* The spawn counter, then the restore request at flags[4]. */
    u8 flags[5];
    u8 count;
    Model672Element streaks[64];
    Model672Element sprites[64];
    Model672Element bursts[64];
} Model672State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model672ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CB18[3];
extern Model672Command D_8013CB6C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

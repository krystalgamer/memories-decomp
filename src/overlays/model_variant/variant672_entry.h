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
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the spark spread, the
 * flare size and two colours (sparks and rings, then flares). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model672Command;

/* One 0x50-byte element: a spark streak, a flare or a ring sprite. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u16 spawned, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
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
    POLY_FT4 quad;
    POLY_GT3 fan;
    GsGLINE line;
    SVECTOR center;
    /* Seventeen points round a circle of the command's radius. */
    SVECTOR rim[17];
    /* The flare square and the ring square. */
    SVECTOR flare[4];
    SVECTOR ring[4];
    SVECTOR origin;
    /* Spark cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count;
    Model672Element sparks[64];
    Model672Element flares[64];
    Model672Element rings[64];
} Model672State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model672ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013CB00[];
#endif
extern GsIMAGE D_8013CB18[3];
extern Model672Command D_8013CB6C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

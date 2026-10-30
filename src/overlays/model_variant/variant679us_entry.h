#ifndef MEMORIES_DECOMP_MODEL_VARIANT679US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT679US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the effect radius, the
 * height and two colours (the shells, then the sparks and bursts). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model679UsCommand;

/* One 0x4C-byte element: a shell, a spark or a burst sprite. */
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
} Model679UsElement;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4], unknown_22;
    POLY_FT4 quad;
    POLY_GT4 band;
    /* A five-by-five lattice: five rings of five points round the axis. */
    SVECTOR grid[5][5];
    /* The spark square. */
    SVECTOR square[4];
    /* The inner and outer rims of the shell's skirt. */
    SVECTOR inner[6];
    SVECTOR outer[6];
    /* The burst square and the trail square. */
    SVECTOR burst[4];
    SVECTOR trail[4];
    /* Shell cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count;
    /* Model joints the shells start from, in spawn order. */
    u8 joints[12];
    /* Texture origins of the eight burst animation frames. */
    u8 frame_u[8];
    u8 frame_v[8];
    VECTOR spark_offsets[5];
    VECTOR trail_offsets[5];
    Model679UsElement shells[64];
    Model679UsElement sparks[64];
    Model679UsElement bursts[64];
} Model679UsState;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model679UsViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013D120[];
#endif
extern GsIMAGE D_8013D138[11];
extern Model679UsCommand D_8013D26C[];

s32 func_8013B004(u8 *context, s32 command);

#endif

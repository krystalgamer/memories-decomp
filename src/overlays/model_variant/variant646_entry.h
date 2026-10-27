#ifndef MEMORIES_DECOMP_MODEL_VARIANT646_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT646_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the size of the glows and
 * shells, then the glow/shard colour and the band/ray colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model646Command;

/* One 0x4C-byte element: a glow, shard, orb, band ring or ray sphere placed
 * through its own coordinate unit. */
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
} Model646Element;

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
    POLY_GT3 shard;
    POLY_GT4 strip;
    GsGLINE line;
    SVECTOR corners[4];
    SVECTOR triangle[3];
    /* Inner and outer ring points, each in the XY and the XZ plane. */
    SVECTOR ring_points[17][2][2];
    /* Eight meridians of nine points on a sphere; the last row is unused. */
    SVECTOR sphere[9][9];
    /* Orb cursor, then the restore request at flags[4]. */
    u8 flags[5];
    u8 copies, orb_count, shard_count;
    /* Per-orb numerator and divisor of the X and Y arcs. */
    s32 arc_x[8][2];
    s32 arc_y[8][2];
    Model646Element glows[2];
    Model646Element shards[16][2];
    Model646Element orbs[8][2];
    Model646Element rings[8][2];
    Model646Element rays[8][2];
} Model646State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model646ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E640[2];
extern Model646Command D_8013E678[];

s32 func_8013B004(u8 *context, s32 command);

#endif

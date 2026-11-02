#ifndef MEMORIES_DECOMP_MODEL_VARIANT660US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT660US_ENTRY_H

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
 * shards, then the glow/shard/orb colour and the band/ray/spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model660UsCommand;

/* One 0x4C-byte element: a glow, shard, orb, band ring, ray sphere or spark
 * placed through its own coordinate unit. */
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
    /* A spark's final offset from its orb; zero for the other elements. */
    VECTOR offset;
} Model660UsElement;

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
    POLY_GT3 shard;
    POLY_GT4 strip;
    GsGLINE line;
    SVECTOR corners[4];
    SVECTOR triangle[3];
    /* Inner and outer ring points, each in the XY and the XZ plane. */
    SVECTOR ring_points[17][2][2];
    /* Eight meridians of nine points on a sphere; the last row is unused. */
    SVECTOR sphere[9][9];
    /* Corners of the small spark quad, a quarter of the radius. */
    SVECTOR spark_corners[4];
    /* Orb cursors of the two copies, then the restore request at flags[4]. */
    u8 flags[5];
    u8 copies, orb_count, shard_count, spark_count;
    /* Per-orb numerator and divisor of the X and Y arcs. */
    s32 arc_x[8][2];
    s32 arc_y[8][2];
    Model660UsElement glows[2];
    Model660UsElement shards[16][2];
    Model660UsElement orbs[8][2];
    Model660UsElement rings[8][2];
    Model660UsElement rays[8][2];
    /* Five sparks per orb and copy. */
    Model660UsElement sparks[5][8][2];
} Model660UsState;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model660UsViewSnapshot;

extern MATRIX D_800FE168;
/* Spark, shard/band and glow/orb texture images; the entry uploads them last
 * to first, so tpage[0] belongs to the glows and orbs. */
extern GsIMAGE gModelVariant660Us_D3674[3];
extern Model660UsCommand gModelVariant660Us_D36C8[];

s32 ModelVariant660Us_HandleRequest(u8 *context, s32 command);

#endif

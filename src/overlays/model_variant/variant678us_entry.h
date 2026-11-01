#ifndef MEMORIES_DECOMP_MODEL_VARIANT678US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT678US_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the radius and the rise height,
 * then the bubble colour and the burst/spray colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model678UsCommand;

/* One 0x4C-byte element: a rising bubble, a burst or a spray, placed through
 * its own coordinate unit. */
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
    /* A bubble's rise: the arc height in vy and the end depth in vz. */
    VECTOR target;
} Model678UsElement;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4];
    u16 unknown_22;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Bubble, burst and spray squares of one, two and half the radius. */
    SVECTOR bubble_corners[4];
    SVECTOR burst_corners[4];
    SVECTOR spray_corners[4];
    /* Inner and outer rings of seventeen points and their midpoints. */
    SVECTOR inner_ring[17];
    SVECTOR outer_ring[17];
    SVECTOR middle_ring[17];
    SVECTOR shard_corners[4];
    /* Bubble cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 rows, columns;
    /* Texture-sheet cells of the burst animation. */
    u8 cell_u[8];
    u8 cell_v[8];
    /* Spray directions, then shard directions, both rotated from the slot. */
    VECTOR spray_axes[5];
    VECTOR shard_axes[16];
    Model678UsElement bubbles[12][2];
    Model678UsElement bursts[12][2];
    Model678UsElement sprays[12][2];
} Model678UsState;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model678UsViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE gModelVariant678Us_D2240[11];
extern Model678UsCommand gModelVariant678Us_D2374[];

s32 ModelVariant678Us_HandleRequest(u8 *context, s32 command);

#endif

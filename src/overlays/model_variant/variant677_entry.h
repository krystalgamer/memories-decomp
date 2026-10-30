#ifndef MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the sprite size, a height
 * and two colours (sprites, then halos and sparks). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model677Command;

/* One 0x4C-byte element: a rising sprite, a halo or a spark. */
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
} Model677Element;

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
    POLY_GT4 fan;
    /* The sprite, halo and spark squares. */
    SVECTOR sprite[4];
    SVECTOR halo[4];
    SVECTOR spark[4];
    /* Two circles of seventeen points and their midpoints in the XY
     * plane. */
    SVECTOR inner[17];
    SVECTOR outer[17];
    SVECTOR middle[17];
    /* The square of the five bursts round each halo. */
    SVECTOR burst[4];
    /* The same circles in the XZ plane. */
    SVECTOR inner_xz[17];
    SVECTOR outer_xz[17];
    SVECTOR middle_xz[17];
    /* Spawn cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 columns;
    u8 rows;
    /* Texture cells of the halo animation. */
    u8 halo_u[8];
    u8 halo_v[8];
    u8 unknown_447;
    /* Drift offsets of the sparks and of the halo bursts. */
    VECTOR drift[5];
    VECTOR spread[5];
    Model677Element sprites[12][2];
    Model677Element halos[12][2];
    Model677Element sparks[12][2];
} Model677State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model677ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D764[11];
extern Model677Command D_8013D898[];

s32 func_8013B004(u8 *context, s32 command);

#endif

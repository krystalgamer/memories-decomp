#ifndef MEMORIES_DECOMP_MODEL_VARIANT652_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT652_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the ring radius, the dome
 * height, then the ring/spark colour and the dome colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model652Command;

/* One 0x4C-byte element: a ring, a dome, a spark or the screen flash. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    /* The sparks keep their outward drift here instead of a scale. */
    VECTOR scale;
    VECTOR position;
    /* The domes keep their row count in vx; the sparks their rise. */
    VECTOR spin;
} Model652Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s32 other_slot;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_22;
    POLY_GT3 fan;
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_F4 flash_poly;
    /* Seventeen points round the ring and, in [1], their rotated copy. */
    SVECTOR rim[17][2];
    /* Seventeen meridians of nine points on the dome. */
    SVECTOR dome[17][9];
    SVECTOR corners[4];
    /* The ring hand-over latch in flags[0]. */
    u8 flags[5];
    u8 ring_count, dome_count, spark_count;
    u8 rim_uv[17][2];
    /* Per-point red, green and blue dome shading. */
    u8 shade[3][17][9];
    s16 cursor;
    SVECTOR origin;
    Model652Element rings[64];
    Model652Element domes[4];
    Model652Element sparks[3][8][4];
    Model652Element flash;
} Model652State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model652ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D798[3];
extern Model652Command D_8013D7EC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

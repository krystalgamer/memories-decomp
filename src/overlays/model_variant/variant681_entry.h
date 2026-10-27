#ifndef MEMORIES_DECOMP_MODEL_VARIANT681_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT681_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x10-byte command row: the start delay, the size of the flash and the
 * height of the rings, then the flash, ring/spark and dome colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} Model681Command;

/* One 0x4C-byte element: a flash, a ring, a burst, a dome or a spark. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    /* Flashes count their frames in rotation.vx. */
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model681Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[6];
    u16 clut[6], unknown_2A;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    SVECTOR ring[17];
    SVECTOR rim[17];
    SVECTOR inner[6][8];
    SVECTOR outer[6][8];
    SVECTOR burst_corners[4];
    SVECTOR dome[17][9];
    SVECTOR cone_inner[9];
    SVECTOR cone_outer[9];
    SVECTOR cone_middle[9];
    SVECTOR spark_corners[4];
    /* [0] rings started, [1] spark row cursor, [2] and [3] spark timers,
     * [5] dome started. */
    u8 flags[6];
    u8 flash_count, column_count, row_count, spark_columns, spark_rows;
    u8 unknown_AA3;
    VECTOR offsets[5];
    Model681Element flashes[2];
    Model681Element rings[4][5];
    Model681Element bursts[4][5];
    Model681Element domes[4];
    Model681Element sparks[32][2];
} Model681State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model681ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013EE38[8];
extern Model681Command D_8013EF18[];

s32 func_8013B004(u8 *context, s32 command);

#endif

#ifndef MEMORIES_DECOMP_MODEL_VARIANT682US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT682US_ENTRY_H

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
#include "../../psyq/abs.h"

/* One 0xC-byte command row: the start delay, the size of the strips and
 * rays, then the strip colour and the ray/fan/spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model682UsCommand;

/* One 0x4C-byte element: a strip column or a ray burst. */
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
} Model682UsElement;

/* One 0x20-byte spark: an animated sprite pinned to a model point. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, unknown_05;
    u16 elapsed, duration;
    u16 unknown_0A;
    GsCOORDUNIT *G32 coord;
    VECTOR position;
} Model682UsSpark;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 tri;
    SVECTOR spokes[17][2];
    SVECTOR rays[33][2];
    SVECTOR fan[17][2];
    /* The square every spark sprite is projected from. */
    SVECTOR quad[4];
    u8 flags[5];
    u8 column_count, burst_count, spark_columns, spark_rows, unknown_4F1[3];
    /* The model-point matrix captured when each strip column spawns. */
    MATRIX matrices[64];
    Model682UsElement columns[64];
    Model682UsElement bursts[8];
    Model682UsSpark sparks[16][6];
} Model682UsState;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model682UsViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE gModelVariant682Us_D20B0[3];
extern Model682UsCommand gModelVariant682Us_D2104[];

s32 ModelVariant682Us_HandleRequest(u8 *context, s32 command);

#endif

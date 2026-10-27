#ifndef MEMORIES_DECOMP_MODEL_VARIANT662_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT662_ENTRY_H

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
#include "../../psyq/abs.h"

/* One 0x10-byte command row: the start delay, the radius and the cone
 * height, then the tube, the cone/dome and the spark colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} Model662Command;

/* One 0x4C-byte element: a tube, a cone, a dome or a spark. */
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
    VECTOR offset;
} Model662Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    /* [ring point][segment][tube] */
    SVECTOR tube_points[9][17][3];
    /* [ring point][band][cone] */
    SVECTOR cone_points[17][9][3];
    /* [ring point][band][dome] */
    SVECTOR dome_points[9][5][3];
    SVECTOR corners[4];
    u8 flags[5];
    u8 tube_count, spark_count;
    /* Set once a tube has opened its cone and sparks. */
    u8 triggered[3], unknown_218E[2];
    Model662Element tubes[3];
    Model662Element cones[3];
    Model662Element domes[3];
    /* [spark][tube] */
    Model662Element sparks[8][3];
} Model662State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model662ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013F534[3];
extern Model662Command D_8013F588[];

s32 func_8013B004(u8 *context, s32 command);

#endif

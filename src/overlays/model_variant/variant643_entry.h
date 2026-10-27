#ifndef MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the size of the rings, then
 * the ring colour and the spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model643Command;

/* One 0x50-byte element: a textured ring, a Gouraud halo or a spark. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Spark column heads only: the row the next respawn reuses. */
    u8 next, unknown_11[3];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR velocity;
} Model643Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 band;
    POLY_G4 halo;
    POLY_FT4 sprite;
    /* Seventeen points around a circle: the top rim, then the bottom rim
     * at four times the radius and the height below. */
    SVECTOR cone[17][2];
    /* Seventeen inner (twice the radius) and outer (five times) points. */
    SVECTOR ring[17][2];
    SVECTOR corners[4];
    /* Spark trigger, then the restore request at flags[4]. */
    u8 flags[5];
    /* Bands, halos, spark rows and spark columns. */
    u8 count[4];
    u8 unknown_2E9[3];
    Model643Element bands[4];
    Model643Element halos[1];
    Model643Element sparks[8][16];
} Model643State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model643ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D2B0[2];
extern Model643Command D_8013D2E8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

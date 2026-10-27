#ifndef MEMORIES_DECOMP_MODEL_VARIANT661_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT661_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the size of the quads and
 * rings, then the streak colour and the burst/spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model661Command;

/* One 0x50-byte element: a streak, a burst or a spark. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u8 done, count, unknown_12[2];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_40;
} Model661Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_G4 quad;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Two edges of each streak's trail: [edge][point][streak]. */
    SVECTOR trail[2][64][8];
    SVECTOR offset;
    SVECTOR prev[2];
    SVECTOR heads[2][8];
    SVECTOR corners[8];
    SVECTOR ring[2][16];
    SVECTOR outer[2][16];
    u8 flags[5];
    u8 rows, cols, burst_count, spark_count, unknown_2381[3];
    Model661Element streaks[8];
    Model661Element bursts[8];
    Model661Element sparks[8][8];
} Model661State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model661ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013EE2C[2];
extern Model661Command D_8013EE64[];

s32 func_8013B004(u8 *context, s32 command);

#endif

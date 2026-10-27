#ifndef MEMORIES_DECOMP_MODEL_VARIANT669_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT669_ENTRY_H

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

/* One 0xE-byte command row: the start delay, the age at which a ring
 * releases its burst, the size of the rings and cones, then the ring/streak
 * colour and the burst colour. */
typedef struct {
    s16 delay;
    s16 release;
    u16 radius, height;
    u8 color[6];
} Model669Command;

/* One 0x54-byte element: a ring, a ring echo, a burst or a streak. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Rings only: the next echo slot and the burst-released flag. */
    u16 echo, released;
    /* Streaks only. */
    u8 done, count, unknown_16[2];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model669Element;

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
    POLY_GT4 strip;
    POLY_GT3 shard;
    POLY_G4 quad;
    SVECTOR inner[33];
    SVECTOR outer[33];
    SVECTOR corners[4];
    SVECTOR ring[17];
    SVECTOR rim[17];
    SVECTOR spikes[4];
    SVECTOR flash_corners[4];
    SVECTOR cone_inner[17];
    SVECTOR cone_outer[17];
    SVECTOR cone_middle[17];
    /* Two edges of each streak's trail: [edge][point][streak]. */
    SVECTOR trail[2][64][2];
    SVECTOR echo_corners[4];
    SVECTOR prev[2];
    SVECTOR heads[2][2];
    SVECTOR offset;
    /* [1] ring cursor, [2] all rings started, [3] first frame seen. */
    u8 flags[5];
    u8 ring_count, echo_count, streak_count, trail_length;
    u8 unknown_E41[3], unknown_E44;
    u8 unknown_E45[3], unknown_E48[4];
    Model669Element rings[8];
    Model669Element echoes[16][8];
    Model669Element bursts[8];
    Model669Element streaks[2];
} Model669State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model669ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013F05C[3];
extern Model669Command D_8013F0B0[];

s32 func_8013B004(u8 *context, s32 command);

#endif

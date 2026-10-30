#ifndef MEMORIES_DECOMP_MODEL_VARIANT633_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT633_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the trail width and height,
 * the trail colour and the spark colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, spark_red, spark_green, spark_blue;
} Model633Command;

/* One 0x50-byte effect record. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    s16 ticks, launched;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model633Element;

/* The work area: the command, the slot, the packets, the swept trail
 * grid (nine spokes of six history rows of twelve trails), the spoke
 * template, two 32-point rings, the spark square, the trail heads, the
 * flags and counts, the per-trail arc parameters and five element banks. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, height;
    u16 tpage;
    u16 unknown_10[2];
    u16 clut;
    u16 unknown_16[3];
    POLY_G4 shaded;
    POLY_FT4 flat;
    GsBOXF box;
    SVECTOR grid[9][6][12];
    SVECTOR spokes[9];
    SVECTOR ring_lower[32];
    SVECTOR square[4];
    SVECTOR ring_upper[32];
    SVECTOR heads[6][12];
    u8 flags[5];
    u8 trail_count, burst_count, spark_count;
    s16 peak[12];
    s16 base[12];
    s16 curve[12];
    s16 depth[12];
    u16 progress[12];
    Model633Element trails[12];
    Model633Element bursts[12];
    Model633Element sparks[3][12];
    Model633Element flares[4][12];
    Model633Element glows[12];
} Model633State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model633ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E29C;
extern Model633Command D_8013E2B8[];

s32 func_8013B004(u8 *context, s32 command);

#endif

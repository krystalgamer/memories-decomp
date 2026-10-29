#ifndef MEMORIES_DECOMP_MODEL_VARIANT628_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT628_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xA-byte command row: the start delay, the dome radius and height
 * and the effect colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, unknown_09;
} Model628Command;

/* One 0x50-byte effect part placed through its own coordinate unit. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* The wobble phase of the domes. */
    u16 stage, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR velocity;
} Model628Part;

/* A ring point and the matching point on the wider, lowered outer ring. */
typedef struct {
    SVECTOR inner, outer;
} Model628RingPoint;

/* The work area: the command, the slot, the packets, the sprite square,
 * the dome grid (17 meridians of 9 points) and its spun copy, the burst
 * ring, the flags and counts, and the four part families. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1A;
    POLY_FT4 flat;
    POLY_GT4 shaded;
    u8 unknown_78[0x24];
    SVECTOR square[4];
    SVECTOR dome[17][9];
    SVECTOR spun[17][9];
    Model628RingPoint ring[17];
    /* The burst launch cursor, then the finish flag at flags[4]. */
    u8 flags[5];
    u8 count, spark_count, dome_count, burst_count;
    u8 unknown_B65[3];
    Model628Part balls[3];
    Model628Part sparks[8][3];
    Model628Part domes[2][3];
    Model628Part bursts[32][3];
} Model628State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model628ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D598[3];
extern Model628Command D_8013D5EC[];

s32 func_8013B004(u8 *context, s32 command);

#endif

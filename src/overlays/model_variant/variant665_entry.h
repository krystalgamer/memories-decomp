#ifndef MEMORIES_DECOMP_MODEL_VARIANT665_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT665_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/libgte.h"
#include "../../psyq/string.h"

/* One 0x1C-byte command row: the start delay, the length of both
 * ambient fades, the time rings keep spawning, the size, and the ring,
 * spark and rising-quad colours. */
typedef struct {
    s16 delay;
    s16 unknown_02;
    s32 fade;
    s32 ring_until;
    u16 radius, height;
    u8 ring_color[3];
    u8 spark_color[3];
    u8 rise_color[3];
    u8 unknown_19[3];
} Model665Command;

/* One 0x4C-byte quad record placed through its own coordinate unit. */
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
    VECTOR growth;
} Model665Quad;

/* One 0x2C-byte ring of strips flying from start to end. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 unknown_04[2];
    u16 elapsed, duration;
    u8 unknown_0A[2];
    VECTOR start;
    VECTOR end;
} Model665Ring;

/* The work area: the command, the timers, the six textures, the strip
 * and quad packets, the vertex tables, the spawn counters, the strip UV
 * cycle, the record counts and the five record groups. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s32 fade_in;
    s32 fade_out;
    s32 focal;
    s16 delay;
    u16 radius, height;
    u16 tpage[6];
    u16 clut[6];
    u16 unknown_36;
    POLY_GT4 strip;
    POLY_FT4 flat;
    SVECTOR ring_inner[17];
    SVECTOR ring_outer[17];
    SVECTOR square[4];
    SVECTOR petals[6][2];
    SVECTOR burst_square[4];
    SVECTOR disc_inner[17];
    SVECTOR disc_outer[17];
    SVECTOR fan_square[4];
    /* Rising row, next spark, next ring, ring timer, restore request,
     * fan count and burst trigger. */
    u8 flags[7];
    u8 fade_state;
    u8 strip_u[5];
    u8 unknown_381[3];
    u8 strip_v[5];
    u8 unknown_389[3];
    u8 ring_count, rows, cols, spark_count, fan_count;
    u8 unknown_391[3];
    Model665Quad rising[8][4];
    Model665Quad sparks[32];
    Model665Quad burst;
    Model665Quad fans[32];
    Model665Ring rings[32];
} Model665State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model665ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013E504[10];
extern Model665Command D_8013E61C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
